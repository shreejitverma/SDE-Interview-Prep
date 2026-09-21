"""Behavior tests for tools/build_mocs.py: run `python3 -m unittest discover tools/tests`."""

import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOL = Path(__file__).resolve().parent.parent / "build_mocs.py"

HAND_WRITTEN = "# Guides\n\nStart with [the first guide](one.md).\n"


def run_tool(repo: Path) -> None:
    subprocess.run([sys.executable, str(TOOL), "--repo", str(repo), "--apply"],
                   check=True, capture_output=True, text=True)


def snapshot(repo: Path) -> dict[str, str]:
    return {str(p.relative_to(repo)): p.read_text() for p in sorted(repo.rglob("*.md"))}


class BuildMocsTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = Path(self.tmp.name)
        files = {
            "Topics/alpha.md": "# Alpha\n",
            "Topics/beta.md": "# Beta\n",
            "Guides/README.md": HAND_WRITTEN,
            "Guides/one.md": "# One\n",
            "Guides/two.md": "# Two\n",
        }
        for rel, text in files.items():
            (self.repo / rel).parent.mkdir(parents=True, exist_ok=True)
            (self.repo / rel).write_text(text)
        subprocess.run(["git", "init", "-q", str(self.repo)], check=True)
        subprocess.run(["git", "-C", str(self.repo), "add", "."], check=True)

    def tearDown(self):
        self.tmp.cleanup()

    def test_rerun_after_commit_changes_nothing(self):
        run_tool(self.repo)
        subprocess.run(["git", "-C", str(self.repo), "add", "."], check=True)  # the generated notes get tracked
        first = snapshot(self.repo)
        run_tool(self.repo)
        self.assertEqual(snapshot(self.repo), first)

    def test_created_note_lists_contents(self):
        run_tool(self.repo)
        text = (self.repo / "Topics/README.md").read_text()
        self.assertIn("type: moc", text)
        self.assertIn("## Contents", text)
        self.assertIn("[Alpha](alpha.md)", text)
        self.assertIn("[Beta](beta.md)", text)

    def test_hand_written_note_keeps_its_text_and_gets_only_unlinked_notes(self):
        run_tool(self.repo)
        text = (self.repo / "Guides/README.md").read_text()
        self.assertTrue(text.startswith(HAND_WRITTEN))
        generated = text[len(HAND_WRITTEN):]
        self.assertIn("## Also in this folder", generated)
        self.assertIn("[Two](two.md)", generated)
        self.assertNotIn("one.md", generated)


if __name__ == "__main__":
    unittest.main()
