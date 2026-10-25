"""Behavior tests for tools/check_shell_labs.py: run `python3 -m unittest discover tools/tests`.

The real linters are replaced by fake executables on PATH that log their arguments,
so these tests pin the tool's own logic (what it checks, how it reports) on any machine.
"""

import os
import shutil
import stat
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOL = Path(__file__).resolve().parent.parent / "check_shell_labs.py"
LABS = "19-Shell-and-Unix-Engineering/labs"
GIT = shutil.which("git")

FAKE = """#!/bin/sh
printf '%s\\n' "$*" >> "{log}/{name}.log"
exit "${{FAKE_{var}_RC:-0}}"
"""

SCRIPT = "#!/bin/sh\necho hi\n"
LIB = "# shellcheck shell=bash\nhelper() { :; }\n"
TEST = "#!/usr/bin/env bats\n\n@test \"x\" {\n  true\n}\n"
ZSH = "#!/usr/bin/env zsh\nprint -r -- hi\n"


class CheckShellLabsTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        self.repo, self.bin, self.log = root / "repo", root / "bin", root / "log"
        for d in (self.repo, self.bin, self.log):
            d.mkdir()
        for name in ("shellcheck", "shfmt", "bats", "zsh"):
            self.fake(name)
        # git is reached through a wrapper so PATH can hold only the fakes.
        self.write_exe(self.bin / "git", f'#!/bin/sh\nexec "{GIT}" "$@"\n')
        subprocess.run([GIT, "init", "-q", str(self.repo)], check=True)

    def tearDown(self):
        self.tmp.cleanup()

    def fake(self, name: str) -> None:
        self.write_exe(self.bin / name, FAKE.format(log=self.log, name=name, var=name.upper()))

    @staticmethod
    def write_exe(path: Path, text: str) -> None:
        path.write_text(text)
        path.chmod(path.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

    def add(self, rel: str, text: str, executable: bool = False, track: bool = True) -> None:
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        if executable:
            self.write_exe(path, text)
        else:
            path.write_text(text)
        if track:
            subprocess.run([GIT, "-C", str(self.repo), "add", rel], check=True)

    def add_lab(self) -> None:
        self.add(f"{LABS}/lib/lab.bash", LIB)
        self.add(f"{LABS}/demo/demo.sh", SCRIPT, executable=True)
        self.add(f"{LABS}/demo/demo.bats", TEST, executable=True)
        self.add(f"{LABS}/matrix/Dockerfile", "FROM alpine:3.22\n")
        self.add(f"{LABS}/README.md", "# Labs\n")

    def run_tool(self, *args: str, **env: str) -> subprocess.CompletedProcess:
        full_env = {"PATH": str(self.bin), "HOME": self.tmp.name, **env}
        return subprocess.run([sys.executable, str(TOOL), "--repo", str(self.repo), *args],
                              capture_output=True, text=True, env=full_env)

    def calls(self, name: str) -> list[str]:
        path = self.log / f"{name}.log"
        return path.read_text().splitlines() if path.exists() else []

    def test_clean_labs_pass_and_each_linter_sees_the_right_files(self):
        self.add_lab()
        self.add(f"{LABS}/demo/prompt.zsh", ZSH, executable=True)
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("result: PASS", result.stdout)
        (sc,) = self.calls("shellcheck")
        for rel in ("lib/lab.bash", "demo/demo.sh", "demo/demo.bats"):
            self.assertIn(f"{LABS}/{rel}", sc)
        self.assertNotIn("prompt.zsh", sc)  # ShellCheck has no zsh support
        self.assertNotIn("Dockerfile", sc)
        (fmt,) = self.calls("shfmt")
        self.assertIn("prompt.zsh", fmt)
        self.assertIn("demo/demo.sh", fmt)
        (bats,) = self.calls("bats")
        self.assertIn(f"{LABS}/demo/demo.bats", bats)
        self.assertEqual(self.calls("zsh"), [f"-n {LABS}/demo/prompt.zsh"])

    def test_linter_failure_fails_the_gate_and_names_the_check(self):
        self.add_lab()
        result = self.run_tool(FAKE_SHELLCHECK_RC="1")
        self.assertEqual(result.returncode, 1)
        self.assertIn("FAIL shellcheck", result.stdout)
        self.assertIn("result: FAIL (shellcheck)", result.stdout)

    def test_bats_failure_fails_the_gate(self):
        self.add_lab()
        result = self.run_tool(FAKE_BATS_RC="1")
        self.assertEqual(result.returncode, 1)
        self.assertIn("result: FAIL (bats)", result.stdout)

    def test_missing_linter_is_an_error_not_a_skip(self):
        self.add_lab()
        (self.bin / "shfmt").unlink()
        result = self.run_tool()
        self.assertEqual(result.returncode, 2)
        self.assertIn("shfmt", result.stderr)
        self.assertIn("brew install", result.stderr)
        self.assertEqual(self.calls("shellcheck"), [])  # nothing runs on a half-installed toolchain

    def test_script_with_shebang_must_be_executable(self):
        self.add_lab()
        self.add(f"{LABS}/demo/other.sh", SCRIPT, executable=False)
        result = self.run_tool()
        self.assertEqual(result.returncode, 1)
        self.assertIn("not executable", result.stdout)
        self.assertIn("demo/other.sh", result.stdout)

    def test_file_must_declare_its_dialect(self):
        self.add_lab()
        self.add(f"{LABS}/lib/bare.sh", "helper() { :; }\n")
        result = self.run_tool()
        self.assertEqual(result.returncode, 1)
        self.assertIn("no shebang or shellcheck shell= directive", result.stdout)

    def test_lab_without_tests_fails(self):
        self.add_lab()
        self.add(f"{LABS}/untested/run.sh", SCRIPT, executable=True)
        result = self.run_tool()
        self.assertEqual(result.returncode, 1)
        self.assertIn("untested: no .bats test", result.stdout)

    def test_untracked_files_are_ignored(self):
        self.add_lab()
        self.add(f"{LABS}/demo/scratch.sh", "not even shell (", track=False)
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertNotIn("scratch.sh", self.calls("shellcheck")[0])

    def test_shebang_without_suffix_is_checked(self):
        self.add_lab()
        self.add(f"{LABS}/demo/tool", "#!/usr/bin/env bash\necho hi\n", executable=True)
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn(f"{LABS}/demo/tool", self.calls("shellcheck")[0])

    def test_no_lab_files_is_a_definitive_empty_state(self):
        self.add("README.md", "# Vault\n")
        result = self.run_tool()
        self.assertEqual(result.returncode, 0)
        self.assertIn("0 files", result.stdout)
        self.assertEqual(self.calls("shellcheck"), [])

    def test_shebang_decides_the_dialect_over_the_suffix(self):
        self.add_lab()
        self.add(f"{LABS}/demo/qualifiers.sh", ZSH, executable=True)
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertEqual(self.calls("zsh"), [f"-n {LABS}/demo/qualifiers.sh"])
        self.assertNotIn("qualifiers.sh", self.calls("shellcheck")[0])

    def test_directive_in_a_non_shell_file_is_ignored(self):
        self.add_lab()
        self.add(f"{LABS}/demo/NOTES.md", "# Notes\n\n```sh\n# shellcheck shell=bash\necho hi\n```\n")
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertNotIn("NOTES.md", self.calls("shellcheck")[0])
        self.assertNotIn("NOTES.md", self.calls("shfmt")[0])

    def test_shells_shellcheck_cannot_name_are_checked_as_their_base_dialect(self):
        self.add_lab()
        self.add(f"{LABS}/demo/tool-ash", "#!/bin/ash\necho hi\n", executable=True)
        self.add(f"{LABS}/demo/tool-ksh93", "#!/usr/bin/env ksh93\necho hi\n", executable=True)
        self.add(f"{LABS}/demo/tool-mksh", "#!/bin/mksh\necho hi\n", executable=True)
        self.add(f"{LABS}/demo/tool-posh", "#!/usr/bin/posh\necho hi\n", executable=True)
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stdout)
        calls = self.calls("shellcheck")
        self.assertIn(f"--format=gcc --shell=busybox {LABS}/demo/tool-ash", calls)
        self.assertIn(f"--format=gcc --shell=ksh {LABS}/demo/tool-ksh93 {LABS}/demo/tool-mksh", calls)
        self.assertIn(f"--format=gcc --shell=sh {LABS}/demo/tool-posh", calls)
        self.assertIn(f"{LABS}/demo/tool-ash", self.calls("shfmt")[0])

    def test_suffixless_ash_script_gets_the_executable_check(self):
        self.add_lab()
        self.add(f"{LABS}/demo/tool-ash", "#!/bin/ash\necho hi\n", executable=False)
        result = self.run_tool()
        self.assertEqual(result.returncode, 1)
        self.assertIn(f"{LABS}/demo/tool-ash: has a shebang but is not executable", result.stdout)


if __name__ == "__main__":
    unittest.main()
