"""Behavior tests for the note-status check in tools/audit_vault.py: run `python3 -m unittest discover tools/tests`."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent.parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

import audit_vault  # noqa: E402
import vault_schema  # noqa: E402

TOOL = TOOLS_DIR / "audit_vault.py"


def note(status_line: str) -> str:
    return f"---\ntype: concept\ntrack: [sde]\nlevel:\n{status_line}\nlast_reviewed:\nsources: []\n---\n\n# Note\n"


class NoteStatusTest(unittest.TestCase):
    def test_reads_plain_and_quoted_values(self):
        self.assertEqual(audit_vault.note_status(note("status: draft")), "draft")
        self.assertEqual(audit_vault.note_status(note('status: "solid"')), "solid")

    def test_blank_and_absent_are_distinguished(self):
        self.assertEqual(audit_vault.note_status(note("status:")), "")
        self.assertIsNone(audit_vault.note_status(note("tags: []")))

    def test_ignores_status_lines_outside_frontmatter(self):
        self.assertIsNone(audit_vault.note_status("# Title\n\nstatus: evergreen\n"))
        self.assertEqual(audit_vault.note_status(note("status: seed") + "\nstatus: evergreen\n"), "seed")

    def test_schema_statuses(self):
        self.assertEqual(vault_schema.STATUSES, ("seed", "draft", "solid", "canonical"))


class StyleCheckTest(unittest.TestCase):
    def run_check(self, files: dict[str, str]) -> subprocess.CompletedProcess:
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            for rel, text in files.items():
                (repo / rel).parent.mkdir(parents=True, exist_ok=True)
                (repo / rel).write_text(text)
            subprocess.run(["git", "init", "-q", str(repo)], check=True)
            subprocess.run(["git", "-C", str(repo), "add", "."], check=True)
            subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@example.com",
                            "commit", "-q", "--no-gpg-sign", "-m", "fixture"], check=True)
            return subprocess.run([sys.executable, str(TOOL), "--repo", str(repo), "--out", "-", "--check", "style"],
                                  capture_output=True, text=True)

    def test_valid_statuses_pass(self):
        files = {f"Topic/{s}.md": note(f"status: {s}") for s in vault_schema.STATUSES}
        files["Topic/README.md"] = note("status: draft")
        result = self.run_check(files)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("invalid_status: 0", result.stdout)

    def test_invalid_missing_and_blank_statuses_fail(self):
        files = {
            "Topic/README.md": note("status: draft"),
            "Topic/old.md": note("status: evergreen"),
            "Topic/blank.md": note("status:"),
            "Topic/missing.md": note("tags: []"),
        }
        result = self.run_check(files)
        self.assertEqual(result.returncode, 1)
        self.assertIn("invalid_status: 3", result.stdout)
        self.assertIn("invalid status 'evergreen'", result.stdout)
        self.assertIn("Topic/missing.md", result.stdout)

    def test_templates_are_not_checked(self):
        files = {"Topic/README.md": note("status: draft"), "Topic/_Templates/App.md": note('status: "researching"')}
        result = self.run_check(files)
        self.assertEqual(result.returncode, 0, result.stdout)


if __name__ == "__main__":
    unittest.main()
