import csv
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import check_aos_coverage as cov

FIELDS = ["id", "kind", "part", "lesson", "concept", "source", "note", "anchor", "lab", "practice", "status"]


class CoverageTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = Path(self.tmp.name)
        self.aos = self.repo / cov.AOS
        (self.aos / "Part-2").mkdir(parents=True)
        (self.aos / "Practice").mkdir()
        (self.aos / "labs" / "lab-05").mkdir(parents=True)

    def tearDown(self):
        self.tmp.cleanup()

    def write_matrix(self, status="done"):
        row = {"id": "L04b-08", "kind": "concept", "part": "2", "lesson": "L04b", "concept": "Ticket lock",
               "source": "slides", "note": "Part-2/L04b.md", "anchor": "Ticket lock", "lab": "labs/lab-05",
               "practice": "Practice/Practice-L04.md", "status": status}
        with open(self.aos / "_coverage.csv", "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=FIELDS)
            w.writeheader()
            w.writerow(row)

    def complete(self):
        (self.aos / "Part-2" / "L04b.md").write_text("# L04b\n\n### Ticket Lock!\n\nbody\n")
        (self.aos / "labs" / "lab-05" / "README.md").write_text("lab\n")
        (self.aos / "labs" / "lab-05" / "Makefile").write_text("all:\n")
        (self.aos / "Practice" / "Practice-L04.md").write_text("Q1 (concepts: L04b-08)\n")

    def test_missing_everything_fails(self):
        self.write_matrix()
        self.assertEqual(cov.main(["--repo", str(self.repo)]), 1)

    def test_complete_row_passes_with_normalized_heading(self):
        self.write_matrix()
        self.complete()
        self.assertEqual(cov.main(["--repo", str(self.repo)]), 0)

    def test_status_must_be_done(self):
        self.write_matrix(status="todo")
        self.complete()
        self.assertEqual(cov.main(["--repo", str(self.repo)]), 1)

    def test_practice_must_cite_row_id(self):
        self.write_matrix()
        self.complete()
        (self.aos / "Practice" / "Practice-L04.md").write_text("no ids here\n")
        problems = cov.run(self.repo)["L04b"][0][1]
        self.assertEqual(problems, ["Practice/Practice-L04.md does not cite L04b-08"])

    def test_summary_never_fails_and_report_is_written(self):
        self.write_matrix()
        self.assertEqual(cov.main(["--repo", str(self.repo), "--summary", "--write-report"]), 0)
        self.assertIn("| L04b | 1 | 0 | 1 |", (self.aos / "00-Coverage.md").read_text())


if __name__ == "__main__":
    unittest.main()
