"""Behavior tests for the private-data guard in tools/audit_pii.py."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOL = Path(__file__).resolve().parent.parent / "audit_pii.py"


def run(repo: Path, *args: str) -> subprocess.CompletedProcess:
    return subprocess.run([sys.executable, str(TOOL), "--repo", str(repo), *args], capture_output=True, text=True)


class GuardTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = Path(self.tmp.name)
        subprocess.run(["git", "init", "-q"], cwd=self.repo, check=True)

    def tearDown(self):
        self.tmp.cleanup()

    def add(self, rel: str, text: str) -> None:
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text)
        subprocess.run(["git", "add", "-f", rel], cwd=self.repo, check=True)

    def test_clean_tree_passes(self):
        self.add("16-Interview-Command-Center/01-Roles/SDE/_Hub.md", "# SDE hub\n")
        result = run(self.repo, "--check")
        self.assertEqual(result.returncode, 0, result.stdout)

    def test_private_location_fails(self):
        self.add("16-Interview-Command-Center/03-Pipeline/Active/X/tracker.md", "stage: onsite\n")
        result = run(self.repo, "--check")
        self.assertEqual(result.returncode, 1)
        self.assertIn("03-Pipeline/Active/X/tracker.md", result.stdout)

    def test_contact_details_fail_without_printing_them(self):
        self.add("16-Interview-Command-Center/01-Roles/SDE/Notes.md", "Recruiter: jane@example.com, 212-555-0100\n")
        result = run(self.repo, "--check")
        self.assertEqual(result.returncode, 1)
        self.assertNotIn("jane@example.com", result.stdout)
        self.assertNotIn("212-555-0100", result.stdout)

    def test_staged_mode_only_sees_staged_files(self):
        self.add("backlog.md", "- task\n")
        subprocess.run(["git", "-c", "user.email=t@t", "-c", "user.name=t", "commit", "-qm", "x"], cwd=self.repo, check=True)
        self.add("notes.md", "fine\n")
        self.assertEqual(run(self.repo, "--check", "--staged").returncode, 0)
        self.assertEqual(run(self.repo, "--check").returncode, 1)

    def test_report_refused_inside_repo(self):
        self.add("a.md", "x\n")
        result = run(self.repo, "--out", str(self.repo / "PII.md"))
        self.assertEqual(result.returncode, 2)
        self.assertFalse((self.repo / "PII.md").exists())


if __name__ == "__main__":
    unittest.main()
