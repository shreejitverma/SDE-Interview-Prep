"""Behavior tests for 18-Developer-Toolchain/code/_harness/verify.py: run `python3 -m unittest discover tools/tests`."""

import importlib.util
import io
import subprocess
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

CODE = Path(__file__).resolve().parents[2] / "18-Developer-Toolchain" / "code"
HARNESS = CODE / "_harness" / "verify.py"
spec = importlib.util.spec_from_file_location("toolchain_verify", HARNESS)
verify = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verify)


class HarnessTest(unittest.TestCase):
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
        rc, out = self.run_harness()
        self.assertEqual(rc, 0)
        self.assertIn("SKIP  pillar/counters (requires pmu)", out)
        self.assertIn("0 passed", out)

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
