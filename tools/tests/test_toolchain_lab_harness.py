"""Behavior tests for 18-Developer-Toolchain/code/_harness/verify.py: run `python3 -m unittest discover tools/tests`."""

import importlib.util
import io
import json
import os
import subprocess
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest import mock

CODE = Path(__file__).resolve().parents[2] / "18-Developer-Toolchain" / "code"
HARNESS = CODE / "_harness" / "verify.py"
spec = importlib.util.spec_from_file_location("toolchain_verify", HARNESS)
verify = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verify)


class LabTreeTestCase(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def lab(self, name: str, script: str, expected: str | None = None) -> Path:
        d = self.root / "pillar" / name
        d.mkdir(parents=True)
        (d / "run.sh").write_text(script)
        if expected is not None:
            (d / "expected").mkdir()
            (d / "expected" / f"{verify.platform_key()}.txt").write_text(expected)
        return d

    def run_harness(self, *args: str) -> tuple[int, str]:
        buf = io.StringIO()
        with redirect_stdout(buf):
            rc = verify.main(["--root", str(self.root), *args])
        return rc, buf.getvalue()


class HarnessTest(LabTreeTestCase):
    def test_empty_tree_passes(self):
        rc, out = self.run_harness()
        self.assertEqual(rc, 0)
        self.assertIn("labs: 0 passed, 0 failed, 0 skipped, 0 unrecorded", out)

    def test_matching_output_passes(self):
        self.lab("hello", "#!/usr/bin/env bash\necho hello\n", "hello\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("PASS  pillar/hello", out)

    def test_changed_output_fails_with_a_diff(self):
        self.lab("hello", "#!/usr/bin/env bash\necho goodbye\n", "hello\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("output differs", out)
        self.assertIn("+goodbye", out)

    def test_failing_script_fails(self):
        self.lab("broken", "#!/usr/bin/env bash\nexit 3\n", "")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("exit 3", out)

    def test_missing_expected_output_fails_until_recorded(self):
        self.lab("new", "#!/usr/bin/env bash\necho fresh\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("MISS  pillar/new", out)
        rc, _ = self.run_harness("--record")
        self.assertEqual(rc, 0)
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)

    def test_unmet_requirement_is_skipped_not_passed(self):
        self.lab("counters", "#!/usr/bin/env bash\n# requires: pmu\necho cycles\n", "cycles\n")
        with mock.patch.dict(os.environ):
            os.environ.pop("TOOLCHAIN_LABS_PMU", None)
            rc, out = self.run_harness()
        self.assertEqual(rc, 0)
        self.assertIn("SKIP  pillar/counters (requires pmu)", out)
        self.assertIn("0 passed", out)

    def test_macos_lab_is_skipped_off_darwin_and_runs_on_darwin(self):
        with mock.patch.object(verify, "platform_key", return_value="test-key"):
            self.lab("xctrace", "#!/usr/bin/env bash\n# requires: macos\necho traced\n", "traced\n")
            with mock.patch.object(verify.platform, "system", return_value="Linux"):
                rc, out = self.run_harness()
            self.assertEqual(rc, 0, out)
            self.assertIn("SKIP  pillar/xctrace (requires macos)", out)
            with mock.patch.object(verify.platform, "system", return_value="Darwin"):
                rc, out = self.run_harness()
            self.assertEqual(rc, 0, out)
            self.assertIn("PASS  pillar/xctrace", out)

    def test_unknown_requirement_fails_the_run(self):
        self.lab("odd", "#!/usr/bin/env bash\n# requires: linx\necho x\n", "x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("unknown requirement: linx", out)
        self.assertIn("1 failed", out)

    def test_nightly_without_rustup_is_skipped_not_a_crash(self):
        self.lab("miri", "#!/usr/bin/env bash\n# requires: nightly\necho x\n", "x\n")
        real_which = verify.shutil.which
        verify.shutil.which = lambda name: None if name == "rustup" else real_which(name)
        try:
            rc, out = self.run_harness()
        finally:
            verify.shutil.which = real_which
        self.assertEqual(rc, 0, out)
        self.assertIn("SKIP  pillar/miri (requires nightly)", out)

    def test_miss_hint_records_only_that_lab(self):
        self.lab("ptrace_debugger", "#!/usr/bin/env bash\necho mine\n")
        capstone = self.lab("capstone_ptrace_debugger", "#!/usr/bin/env bash\necho new\n", "golden\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        hint = next(line for line in out.splitlines() if line.startswith("MISS"))
        self.assertIn("LAB=pillar/ptrace_debugger/", hint)
        needle = hint.split("LAB=", 1)[1].split()[0]
        rc, out = self.run_harness("--record", needle)
        self.assertEqual(rc, 0, out)
        self.assertIn("REC   pillar/ptrace_debugger", out)
        self.assertNotIn("capstone_ptrace_debugger", out)
        self.assertEqual((capstone / "expected" / f"{verify.platform_key()}.txt").read_text(), "golden\n")

    def test_hung_requirement_probe_is_unmet_not_a_hang(self):
        real_which, real_run = verify.shutil.which, verify.subprocess.run
        calls = []

        def hang(cmd, **kwargs):
            calls.append(kwargs.get("timeout"))
            raise subprocess.TimeoutExpired(cmd, kwargs.get("timeout"))

        verify.shutil.which = lambda name: "/usr/bin/" + name
        verify.subprocess.run = hang
        try:
            missing = verify.unmet(["docker", "nightly"])
        finally:
            verify.shutil.which, verify.subprocess.run = real_which, real_run
        self.assertEqual(missing, ["docker", "nightly"])
        self.assertEqual(len(calls), 2)
        self.assertTrue(all(t is not None and t > 0 for t in calls), calls)

    def test_addresses_and_temp_paths_are_scrubbed(self):
        self.assertEqual(verify.scrub("ptr=0x7ffee3b2c9a0 at /tmp/abc/x.c:3"), "ptr=0xADDR at /TMP:3")

    def test_needle_filters_labs(self):
        self.lab("alpha", "#!/usr/bin/env bash\necho a\n", "a\n")
        self.lab("beta", "#!/usr/bin/env bash\necho b\n", "b\n")
        _, out = self.run_harness("alpha")
        self.assertIn("pillar/alpha", out)
        self.assertNotIn("pillar/beta", out)


FAKE_DOCKER = """#!{python}
# A stand-in for the docker CLI: logs its argv, then acts as fake_docker.json says.
import json, sys, time
from pathlib import Path

here = Path(__file__).resolve().parent
config = json.loads((here / "fake_docker.json").read_text())
args = sys.argv[1:]
with (here / "docker_calls.jsonl").open("a") as log:
    log.write(json.dumps(args) + "\\n")
if args[0] == "info":
    sys.stderr.write(config.get("info_stderr", ""))
    sys.exit(config.get("info_rc", 0))
if args[:2] == ["image", "inspect"]:
    sys.exit(config.get("inspect_rc", 0))
if args[0] == "build":
    sys.stderr.write(config.get("build_stderr", ""))
    sys.exit(config.get("build_rc", 0))
if args[0] == "rm":
    sys.exit(0)
if args[0] == "run" and args[-2:] == ["uname", "-m"]:
    sys.stderr.write(config.get("uname_stderr", ""))
    print(config.get("uname", "aarch64"))
    sys.exit(config.get("uname_rc", 0))
if args[0] == "run":
    time.sleep(config.get("run_sleep", 0))
    sys.stdout.write(config.get("run_stdout", ""))
    sys.stderr.write(config.get("run_stderr", ""))
    sys.exit(config.get("run_rc", 0))
sys.exit("fake docker: unexpected call " + repr(args))
"""
LINUX_LAB = "#!/usr/bin/env bash\n# requires: {}\necho native-run\n"


class ContainerDispatchTest(LabTreeTestCase):
    """Linux labs on a non-Linux host, through main() with a fake docker first on PATH."""

    def setUp(self):
        super().setUp()
        self.tools = tempfile.TemporaryDirectory()
        self.bin = Path(self.tools.name)
        (self.bin / "docker").write_text(FAKE_DOCKER.format(python=sys.executable))
        (self.bin / "docker").chmod(0o755)
        self.configure()
        self.addCleanup(self.tools.cleanup)
        for patcher in (
            mock.patch.object(verify.platform, "system", return_value="Darwin"),
            mock.patch.dict(os.environ, {"PATH": f"{self.bin}{os.pathsep}{os.environ['PATH']}"}),
        ):
            patcher.start()
            self.addCleanup(patcher.stop)
        for switch in ("TOOLCHAIN_LABS_CONTAINER", "TOOLCHAIN_LABS_PMU", "TOOLCHAIN_LABS_NETWORK"):
            os.environ.pop(switch, None)

    def configure(self, **config):
        (self.bin / "fake_docker.json").write_text(json.dumps(config))

    def calls(self) -> list[list[str]]:
        log = self.bin / "docker_calls.jsonl"
        return [json.loads(line) for line in log.read_text().splitlines()] if log.exists() else []

    def lab_runs(self) -> list[list[str]]:
        return [c for c in self.calls() if c[0] == "run" and c[-2:] != ["uname", "-m"]]

    def linux_lab(self, name: str = "probe", requires: str = "linux", **expected: str) -> Path:
        """expected maps a platform key, with _ for -, to the content of its expected file."""
        d = self.lab(name, LINUX_LAB.format(requires))
        (d / "expected").mkdir()
        for key, text in expected.items():
            (d / "expected" / f"{key.replace('_', '-')}.txt").write_text(text)
        return d

    def test_dispatches_with_least_privilege_and_uses_the_containers_expected_file(self):
        lab = self.linux_lab(linux_aarch64="from container\n", darwin_arm64="wrong\n", linux_amd64="wrong\n")
        self.configure(run_stdout="from container\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("PASS  pillar/probe", out)
        self.assertIn("linux-aarch64", out)
        self.assertEqual(len(self.lab_runs()), 1)
        argv = self.lab_runs()[0]
        mounts = [argv[i + 1] for i, a in enumerate(argv) if a == "--mount"]
        self.assertEqual(len(mounts), 1)
        fields = mounts[0].split(",")
        self.assertIn(f"source={lab.resolve()}", fields)
        self.assertIn("readonly", fields)
        self.assertNotIn("--privileged", argv)
        self.assertNotIn("--cap-add", argv)
        self.assertEqual(argv[argv.index("--cap-drop") + 1], "ALL")
        self.assertEqual(argv[argv.index("--ulimit") + 1], "core=-1")
        self.assertEqual(argv[argv.index("--network") + 1], "none")
        self.assertIn("--rm", argv)
        self.assertTrue(any(a.startswith("toolchain-labs:") for a in argv), argv)
        self.assertIn("bash run.sh", argv[-1])
        self.assertIn("/tmp/lab", argv[-1], "the lab must run on a writable copy, not the mount")

    def test_x86_container_selects_the_amd64_file(self):
        self.linux_lab(linux_aarch64="wrong\n", linux_amd64="from amd64\n")
        self.configure(uname="x86_64", run_stdout="from amd64\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("PASS  pillar/probe", out)

    def test_changed_output_in_the_container_fails_with_a_diff(self):
        self.linux_lab(linux_aarch64="golden\n")
        self.configure(run_stdout="drifted\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("output differs", out)
        self.assertIn("expected/linux-aarch64.txt", out)
        self.assertIn("+drifted", out)

    def test_missing_container_recording_is_a_miss_naming_the_container_platform(self):
        self.linux_lab(darwin_arm64="host file does not count\n")
        self.configure(run_stdout="x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("MISS  pillar/probe (no expected/linux-aarch64.txt", out)

    def test_record_writes_the_scrubbed_container_platform_file(self):
        lab = self.linux_lab()
        self.configure(run_stdout="ptr=0x7ffee3b2c9a0\n")
        rc, out = self.run_harness("--record")
        self.assertEqual(rc, 0, out)
        self.assertIn("REC   pillar/probe -> expected/linux-aarch64.txt", out)
        self.assertEqual([f.name for f in (lab / "expected").iterdir()], ["linux-aarch64.txt"])
        self.assertEqual((lab / "expected" / "linux-aarch64.txt").read_text(), "ptr=0xADDR\n")

    def test_no_docker_executable_is_a_skip_with_the_reason(self):
        self.linux_lab(linux_aarch64="x\n")
        with tempfile.TemporaryDirectory() as empty, mock.patch.dict(os.environ, {"PATH": empty}):
            rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("SKIP  pillar/probe (requires linux: host is darwin and there is no docker executable", out)
        self.assertIn("0 passed, 0 failed, 1 skipped", out)

    def test_unreachable_daemon_is_a_skip_with_the_daemons_error(self):
        self.linux_lab(linux_aarch64="x\n")
        self.configure(info_rc=1, info_stderr="Cannot connect to the Docker daemon\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("SKIP  pillar/probe (requires linux:", out)
        self.assertIn("Cannot connect to the Docker daemon", out)
        self.assertEqual(self.lab_runs(), [])

    def test_switch_disables_dispatch_without_touching_docker(self):
        self.linux_lab(linux_aarch64="x\n")
        with mock.patch.dict(os.environ, {"TOOLCHAIN_LABS_CONTAINER": "0"}):
            rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("SKIP  pillar/probe (requires linux:", out)
        self.assertIn("TOOLCHAIN_LABS_CONTAINER=0", out)
        self.assertEqual(self.calls(), [])

    def test_docker_run_failure_fails_and_shows_stderr(self):
        self.linux_lab(linux_aarch64="x\n")
        self.configure(run_rc=125, run_stderr="docker: Error response from daemon: mount denied\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("FAIL  pillar/probe (exit 125)", out)
        self.assertIn("mount denied", out)
        self.assertIn("1 failed, 0 skipped", out)

    def test_lab_failure_in_the_container_fails_and_shows_stderr(self):
        self.linux_lab(linux_aarch64="x\n")
        self.configure(run_rc=3, run_stderr="no core file was produced\n", run_stdout="x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("FAIL  pillar/probe (exit 3)", out)
        self.assertIn("no core file was produced", out)

    def test_image_is_built_once_when_absent_and_not_when_present(self):
        self.linux_lab("one", linux_aarch64="x\n")
        self.linux_lab("two", linux_aarch64="x\n")
        self.configure(inspect_rc=1, run_stdout="x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        builds = [c for c in self.calls() if c[0] == "build"]
        self.assertEqual(len(builds), 1)
        self.assertEqual(Path(builds[0][builds[0].index("-f") + 1]).name, "linux-labs.Dockerfile")
        self.assertTrue(Path(builds[0][builds[0].index("-f") + 1]).is_file())
        self.assertEqual(builds[0][builds[0].index("-t") + 1], self.lab_runs()[0][-4])
        (self.bin / "docker_calls.jsonl").unlink()
        self.configure(run_stdout="x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertEqual([c for c in self.calls() if c[0] == "build"], [])

    def test_image_build_failure_fails_every_linux_lab_and_shows_stderr(self):
        self.linux_lab("one", linux_aarch64="x\n")
        self.linux_lab("two", linux_aarch64="x\n")
        self.configure(inspect_rc=1, build_rc=1, build_stderr="E: Unable to locate package gdb\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("FAIL  pillar/one (docker build failed, exit 1", out)
        self.assertIn("FAIL  pillar/two (docker build failed, exit 1", out)
        self.assertIn("Unable to locate package gdb", out)
        self.assertIn("0 passed, 2 failed, 0 skipped", out)
        self.assertEqual(len([c for c in self.calls() if c[0] == "build"]), 1)
        self.assertEqual(self.lab_runs(), [])

    def test_unreadable_container_arch_fails(self):
        self.linux_lab(linux_aarch64="x\n")
        self.configure(uname_rc=125, uname="", uname_stderr="exec format error\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("FAIL  pillar/probe (could not read the container arch", out)
        self.assertIn("exec format error", out)

    def test_timeout_fails_and_removes_the_container_it_named(self):
        self.linux_lab(linux_aarch64="x\n")
        self.configure(run_sleep=30)
        with mock.patch.object(verify, "TIMEOUT_S", 0.5):
            rc, out = self.run_harness()
        self.assertEqual(rc, 1)
        self.assertIn("FAIL  pillar/probe (timed out after 0.5s", out)
        argv = self.lab_runs()[0]
        name = argv[argv.index("--name") + 1]
        self.assertIn(["rm", "-f", name], self.calls())

    def test_macos_labs_never_dispatch(self):
        self.lab("native", "#!/usr/bin/env bash\n# requires: macos\necho native-run\n", "native-run\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("PASS  pillar/native", out)
        self.assertEqual(self.calls(), [])

    def test_a_lab_asking_for_both_linux_and_macos_skips_without_docker(self):
        self.linux_lab(requires="linux, macos", linux_aarch64="x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("SKIP  pillar/probe (requires linux)", out)
        self.assertEqual(self.calls(), [])

    def test_requirements_a_container_cannot_meet_skip_even_when_opted_in(self):
        for token in ("pmu", "nightly", "docker"):
            self.linux_lab(token, requires=f"linux, {token}", linux_aarch64="x\n")
        with mock.patch.dict(os.environ, {"TOOLCHAIN_LABS_PMU": "1"}):
            rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        for token in ("pmu", "nightly", "docker"):
            self.assertIn(f"SKIP  pillar/{token} (requires {token} (", out)
        self.assertIn("0 passed, 0 failed, 3 skipped", out)
        self.assertEqual(self.calls(), [])

    def test_network_stays_opt_in_and_opening_it_drops_the_network_flag(self):
        self.linux_lab(requires="linux, network", linux_aarch64="x\n")
        self.configure(run_stdout="x\n")
        rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("SKIP  pillar/probe (requires network)", out)
        self.assertEqual(self.calls(), [])
        with mock.patch.dict(os.environ, {"TOOLCHAIN_LABS_NETWORK": "1"}):
            rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("PASS  pillar/probe", out)
        self.assertNotIn("--network", self.lab_runs()[0])

    def test_linux_host_runs_natively_without_docker(self):
        lab = self.linux_lab()
        with mock.patch.object(verify.platform, "system", return_value="Linux"):
            (lab / "expected" / f"{verify.platform_key()}.txt").write_text("native-run\n")
            rc, out = self.run_harness()
        self.assertEqual(rc, 0, out)
        self.assertIn("PASS  pillar/probe", out)
        self.assertEqual(self.calls(), [])


class LabGitignoreTest(unittest.TestCase):
    def ignored(self, rel: str) -> bool:
        proc = subprocess.run(["git", "check-ignore", "--no-index", "-q", rel], cwd=CODE)
        self.assertIn(proc.returncode, (0, 1))
        return proc.returncode == 0

    def test_core_dumps_are_ignored_but_core_sources_are_not(self):
        self.assertTrue(self.ignored("02-Version-Control/mini_git/core.12345"))
        self.assertTrue(self.ignored("03-Debugging/drill_uaf_core/core"))
        for source in ("02-Version-Control/mini_git/src/core.rs", "03-Debugging/lab/core.py",
                       "03-Debugging/lab/core.cpp", "02-Version-Control/mini_git/src/core/mod.rs"):
            self.assertFalse(self.ignored(source), source)


if __name__ == "__main__":
    unittest.main()
