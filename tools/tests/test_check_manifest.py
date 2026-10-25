"""Behavior tests for tools/check_manifest.py and the lab-tree vendoring exemption in tools/audit_vault.py.

Run `python3 -m unittest discover tools/tests`.
"""

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent.parent
MANIFEST_TOOL = TOOLS_DIR / "check_manifest.py"
AUDIT_TOOL = TOOLS_DIR / "audit_vault.py"
BOOK = "18-Developer-Toolchain"
HEADER = "order\tpillar_label\tpillar_title\tnote_id\tkind\tpath"


def note(kind: str) -> str:
    return f"---\ntype: {kind}\ntrack: [sde]\nlevel:\nstatus: seed\nlast_reviewed:\nsources: []\n---\n\n# Note\n"


NOTES = {
    "README.md": "moc",
    "00-Program-Execution/README.md": "moc",
    "00-Program-Execution/ELF-and-Mach-O.md": "concept",
    "00-Program-Execution/Drill-Program-Execution-Broken-Rpath.md": "problem",
}


def rows(notes: dict[str, str]) -> list[str]:
    return [f"{i}\t00\tProgram Execution\t{p[:-3]}\t{k}\t{p}" for i, (p, k) in enumerate(notes.items(), start=1)]


class ManifestTest(unittest.TestCase):
    def make_repo(self, notes: dict[str, str], manifest_lines: list[str] | None) -> Path:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        repo = Path(tmp.name)
        for rel, kind in notes.items():
            path = repo / BOOK / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(note(kind))
        if manifest_lines is not None:
            (repo / BOOK / "manifest.tsv").write_text("\n".join(manifest_lines) + "\n")
        subprocess.run(["git", "init", "-q", str(repo)], check=True)
        subprocess.run(["git", "-C", str(repo), "add", "."], check=True)
        return repo

    def run_tool(self, repo: Path) -> subprocess.CompletedProcess:
        return subprocess.run([sys.executable, str(MANIFEST_TOOL), "--repo", str(repo), "--book", BOOK],
                              capture_output=True, text=True)

    def test_complete_manifest_passes(self):
        result = self.run_tool(self.make_repo(NOTES, [HEADER, *rows(NOTES)]))
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("result: PASS", result.stdout)

    def test_missing_book_folder_is_not_an_error(self):
        result = self.run_tool(self.make_repo({}, None))
        self.assertEqual(result.returncode, 0)

    def test_missing_manifest_fails(self):
        result = self.run_tool(self.make_repo(NOTES, None))
        self.assertEqual(result.returncode, 1)
        self.assertIn("manifest.tsv is missing", result.stdout)

    def test_unlisted_note_fails(self):
        listed = dict(list(NOTES.items())[:-1])
        result = self.run_tool(self.make_repo(NOTES, [HEADER, *rows(listed)]))
        self.assertEqual(result.returncode, 1)
        self.assertIn("Drill-Program-Execution-Broken-Rpath.md is not in manifest.tsv", result.stdout)

    def test_row_for_missing_note_fails(self):
        extra = {**NOTES, "00-Program-Execution/Ghost.md": "concept"}
        result = self.run_tool(self.make_repo(NOTES, [HEADER, *rows(extra)]))
        self.assertEqual(result.returncode, 1)
        self.assertIn("Ghost.md is not a tracked note", result.stdout)

    def test_duplicate_row_and_order_gap_fail(self):
        lines = rows(NOTES)
        lines.append(lines[1].replace("2\t", "6\t", 1))
        result = self.run_tool(self.make_repo(NOTES, [HEADER, *lines]))
        self.assertEqual(result.returncode, 1)
        self.assertIn("listed more than once", result.stdout)
        self.assertIn("expected 5", result.stdout)

    def test_kind_must_match_frontmatter_type(self):
        lines = rows(NOTES)
        lines[2] = lines[2].replace("\tconcept\t", "\tplaybook\t")
        result = self.run_tool(self.make_repo(NOTES, [HEADER, *lines]))
        self.assertEqual(result.returncode, 1)
        self.assertIn("does not match frontmatter type 'concept'", result.stdout)

    def test_wrong_header_fails(self):
        result = self.run_tool(self.make_repo(NOTES, ["order\tpath", *rows(NOTES)]))
        self.assertEqual(result.returncode, 1)
        self.assertIn("header must be", result.stdout)

    def test_archived_notes_are_not_required(self):
        notes = {**NOTES, "00-Program-Execution/_archive/Old.md": "concept"}
        result = self.run_tool(self.make_repo(notes, [HEADER, *rows(NOTES)]))
        self.assertEqual(result.returncode, 0, result.stdout)


class LabTreeVendoringTest(unittest.TestCase):
    def test_lab_cargo_toml_is_not_a_vendored_repo(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            files = {
                f"{BOOK}/README.md": note("moc"),
                f"{BOOK}/code/version-control/mini_git/Cargo.toml": "[package]\nname = \"mini_git\"\n",
                "Imported/thing/Cargo.toml": "[package]\nname = \"thing\"\n",
            }
            for rel, text in files.items():
                (repo / rel).parent.mkdir(parents=True, exist_ok=True)
                (repo / rel).write_text(text)
            subprocess.run(["git", "init", "-q", str(repo)], check=True)
            subprocess.run(["git", "-C", str(repo), "add", "."], check=True)
            subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@example.com",
                            "commit", "-q", "--no-gpg-sign", "-m", "fixture"], check=True)
            out = subprocess.run([sys.executable, str(AUDIT_TOOL), "--repo", str(repo), "--out", "-", "--json"],
                                 capture_output=True, text=True, check=True).stdout
            self.assertEqual(json.loads(out)["vendored_dirs"], 1)


if __name__ == "__main__":
    unittest.main()
