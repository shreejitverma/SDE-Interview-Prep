"""Behavior tests for tools/cloud_batch.py (offline; no API calls): `python3 -m unittest discover tools/tests`."""

import contextlib
import io
import json
import subprocess
import sys
import tempfile
import unittest
from argparse import Namespace
from types import SimpleNamespace
from pathlib import Path

TOOLS = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(TOOLS))

import cloud_batch as cb  # noqa: E402

SUMMARY = """# TOC

## Phase 1

### [Volume 00 - Foundations](./Volume_00_Foundations/)
1. [Hypervisors](./Volume_00_Foundations/Chapter_01_Hypervisors.md) - Nitro and Firecracker.
2. [Fabrics](./Volume_00_Foundations/Chapter_02_Fabrics.md) - Clos and Jupiter.

### [Volume 13 - Papers](./Volume_13_Papers/)
1. [Dynamo](./Volume_13_Papers/Chapter_01_Dynamo.md) - The 2007 paper.

## Appendices

- [Appendix A: Glossary](./Appendices/Appendix_A_Glossary.md) - Terms.
"""

FULL_BODY = "# Fabrics\n\n" + "\n\n".join(
    f"## {h}\n\nLine about {h}." for h in
    ("TL;DR", "Mental Model", "Internals", "Numbers to Know", "AWS vs GCP Mechanism Parity",
     "Worked Example: Sizing a spine", "Failure Modes and Pitfalls", "Security and Cost Angles",
     "Interview Questions and Model Answers", "Further Reading"))


def message(text: str, stop: str = "end_turn", urls=(), searches: int = 2) -> dict:
    return {
        "model": "claude-opus-5-5",
        "stop_reason": stop,
        "content": [
            {"type": "server_tool_use", "name": "web_search", "input": {"query": "q"}},
            {"type": "text", "text": text,
             "citations": [{"type": "web_search_result_location", "url": u, "title": "t"} for u in urls]},
        ],
        "usage": {"input_tokens": 1_000_000, "output_tokens": 100_000, "cache_read_input_tokens": 0,
                  "cache_creation_input_tokens": 0, "server_tool_use": {"web_search_requests": searches}},
    }


class Repo:
    """A throwaway vault with the curriculum SUMMARY, STYLE, and stubs."""

    def __init__(self, test: unittest.TestCase):
        self.tmp = tempfile.TemporaryDirectory()
        test.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        cur = self.root / cb.CURRICULUM
        cur.mkdir()
        (cur / "SUMMARY.md").write_text(SUMMARY)
        (cur / "STYLE.md").write_text("# Style\n")
        subprocess.run(["git", "init", "-q", str(self.root)], check=True)
        self.chapters = cb.parse_summary(SUMMARY)

    def write(self, rel: str, text: str) -> None:
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text)


class ParseSummaryTest(unittest.TestCase):
    def test_volumes_chapters_and_appendices(self):
        chs = cb.parse_summary(SUMMARY)
        self.assertEqual([c.cid for c in chs], ["v00c01", "v00c02", "v13c01", "appA"])
        self.assertEqual(chs[1].path, f"{cb.CURRICULUM}/Volume_00_Foundations/Chapter_02_Fabrics.md")
        self.assertEqual(chs[1].scope, "Clos and Jupiter.")
        self.assertEqual(chs[1].volume, "Volume 00 - Foundations")
        self.assertTrue(chs[3].is_appendix)
        self.assertEqual(cb.note_type(chs[2]), "paper")
        self.assertIn("distinguished", cb.note_tracks(chs[2]))

    def test_duplicate_ids_rejected(self):
        dup = SUMMARY + "3. [Again](./Volume_00_Foundations/Chapter_01_Again.md) - x\n"
        with self.assertRaises(ValueError):
            cb.parse_summary(dup)

    def test_parses_the_live_curriculum_summary_when_present(self):
        live = TOOLS.parent / cb.CURRICULUM / "SUMMARY.md"
        if not live.exists():
            self.skipTest("curriculum SUMMARY.md not in this checkout")
        self.assertGreater(len(cb.parse_summary(live.read_text())), 100)


class ExtractAndCleanTest(unittest.TestCase):
    def test_drops_preamble_and_splits_unverified_tail(self):
        text = "Let me research.\n# Fabrics\n\nBody line.\n<!-- UNVERIFIED -->\n- claim one\n"
        body, tail = cb.extract_note(text, "Fabrics")
        self.assertTrue(body.startswith("# Fabrics"))
        self.assertNotIn("UNVERIFIED", body)
        self.assertIn("claim one", tail)

    def test_no_title_line_returns_empty_body(self):
        body, _ = cb.extract_note("no heading here", "Fabrics")
        self.assertEqual(body, "")

    def test_missing_sections_accepts_subtitles(self):
        self.assertEqual(cb.missing_sections(FULL_BODY), [])
        self.assertEqual(cb.missing_sections("# T\n\n## TL;DR\n"), list(cb.SECTIONS[1:]))

    def test_sanitize_links_keeps_allowed_and_unlinks_the_rest(self):
        allowed = {"17-Cloud-AI-Engineering/Volume_00_Foundations/Chapter_01_Hypervisors"}
        body = ("See [[17-Cloud-AI-Engineering/Volume_00_Foundations/Chapter_01_Hypervisors|Hypervisors]] "
                "and [[Made Up Note|the made up one]].\n"
                "| a | [[17-Cloud-AI-Engineering/Volume_00_Foundations/Chapter_01_Hypervisors\\|H]] |\n"
                "| b | [[Nope\\|N]] |\n")
        out, dropped = cb.sanitize_links(body, allowed)
        self.assertIn("[[17-Cloud-AI-Engineering/Volume_00_Foundations/Chapter_01_Hypervisors|Hypervisors]]", out)
        self.assertIn("\\|H]]", out)
        self.assertIn("and the made up one.", out)
        self.assertIn("| b | N |", out)
        self.assertEqual(dropped, ["Made Up Note", "Nope"])

    def test_clean_style_removes_em_dashes(self):
        self.assertEqual(cb.clean_style("a \u2014 b\u2014c"), "a - b - c")


class CostTest(unittest.TestCase):
    def test_usage_cost_applies_batch_discount_and_search_fee(self):
        usage = {"input_tokens": 1_000_000, "output_tokens": 1_000_000, "cache_read_input_tokens": 1_000_000,
                 "cache_creation_input_tokens": 0, "server_tool_use": {"web_search_requests": 100}}
        # Opus 5.5: (4 + 20 + 0.4) * 0.5 = 12.2, plus 100 searches * $0.01.
        self.assertAlmostEqual(cb.usage_cost("claude-opus-5-5", usage), 13.2)

    def test_estimate_low_not_above_high(self):
        repo = Repo(self)
        todo = repo.chapters[:2]
        reqs = cb.build_requests("draft", repo.root, repo.chapters, todo)
        lo, hi = cb.estimate("draft", reqs)
        self.assertGreater(lo, 0)
        self.assertLessEqual(lo, hi)


class RequestTest(unittest.TestCase):
    def test_draft_requests_share_a_cached_system_prefix(self):
        repo = Repo(self)
        reqs = cb.build_requests("draft", repo.root, repo.chapters, repo.chapters)
        self.assertEqual([r["custom_id"] for r in reqs], ["draft-v00c01", "draft-v00c02", "draft-v13c01", "draft-appA"])
        p = reqs[0]["params"]
        self.assertEqual(p["model"], "claude-opus-5-5")
        self.assertEqual(p["output_config"], {"effort": "high"})
        self.assertEqual(p["tools"][0]["type"], "web_search_20260209")
        self.assertEqual(p["system"][-1]["cache_control"], {"type": "ephemeral"})
        self.assertIs(reqs[0]["params"]["system"], reqs[1]["params"]["system"])
        self.assertIn("ten-section chapter template does not apply", json.dumps(reqs[3]["params"]["system"]))
        for r in reqs:
            self.assertRegex(r["custom_id"], r"^[a-zA-Z0-9_-]{1,64}$")
            self.assertNotIn("thinking", r["params"])

    def test_cards_requests_have_no_tools(self):
        repo = Repo(self)
        repo.write(repo.chapters[1].path, cb.render_note(repo.chapters[1], "draft", FULL_BODY, []))
        reqs = cb.build_requests("cards", repo.root, repo.chapters, [repo.chapters[1]])
        self.assertNotIn("tools", reqs[0]["params"])
        self.assertNotIn("status: draft", reqs[0]["params"]["messages"][0]["content"])


class SelectAndApplyTest(unittest.TestCase):
    def setUp(self):
        self.repo = Repo(self)
        self.state = cb.load_state(self.repo.root)
        self.ch = self.repo.chapters[1]

    def test_draft_selects_missing_and_seed_only(self):
        r = self.repo
        r.write(r.chapters[0].path, cb.render_note(r.chapters[0], "solid", "# Hypervisors\n", []))
        r.write(r.chapters[1].path, cb.render_note(r.chapters[1], "seed", "# Fabrics\n", []))
        picked = [c.cid for c in cb.select("draft", r.root, r.chapters, self.state)]
        self.assertEqual(picked, ["v00c02", "v13c01", "appA"])

    def test_in_flight_chapters_are_not_resubmitted(self):
        self.state["batches"].append({"id": "b1", "phase": "draft", "custom_ids": {"draft-v00c02": self.ch.path}})
        picked = [c.cid for c in cb.select("draft", self.repo.root, self.repo.chapters, self.state)]
        self.assertNotIn("v00c02", picked)

    def test_draft_result_writes_note_with_frontmatter_and_sources(self):
        r, report = self.repo, []
        allowed = {c.link for c in r.chapters}
        text = "Researching.\n" + FULL_BODY + "\n<!-- UNVERIFIED -->\n- none\n"
        cost = cb.apply_result("draft", self.ch, message(text, urls=["https://a", "https://a", "https://b"]),
                               r.root, self.state, allowed, True, report)
        note = (r.root / self.ch.path).read_text()
        self.assertTrue(note.startswith("---\ntype: concept\ntrack: [cloud-ai]\nlevel:\nstatus: draft\n"))
        self.assertIn('  - "https://a"\n  - "https://b"\n---', note)
        self.assertNotIn("Researching", note)
        self.assertEqual(cb.note_status(r.root / self.ch.path), "draft")
        self.assertTrue(cb.owned_unchanged(r.root, self.ch, self.state))
        self.assertIn("missing sections none", report[0])
        self.assertAlmostEqual(cost, (4 + 2) * 0.5 + 0.02)

    def test_draft_never_overwrites_a_note_written_meanwhile(self):
        r, report = self.repo, []
        r.write(self.ch.path, cb.render_note(self.ch, "solid", "# Fabrics\nhand written\n", []))
        cb.apply_result("draft", self.ch, message(FULL_BODY), r.root, self.state, set(), True, report)
        self.assertIn("hand written", (r.root / self.ch.path).read_text())
        self.assertIn("skipped", report[0])

    def test_truncated_result_is_not_written(self):
        r, report = self.repo, []
        cb.apply_result("draft", self.ch, message(FULL_BODY, stop="max_tokens"), r.root, self.state, set(), True, report)
        self.assertFalse((r.root / self.ch.path).exists())
        self.assertIn("stop_reason=max_tokens", report[0])

    def test_verify_promotes_to_solid_only_when_unchanged_and_resolved(self):
        r, report = self.repo, []
        cb.apply_result("draft", self.ch, message(FULL_BODY, urls=["https://a"]), r.root, self.state, set(), True, report)
        fixed = FULL_BODY.replace("Line about Internals.", "Line about Internals, corrected.")
        verified = fixed + "\n<!-- VERIFICATION REPORT -->\n- CORRECTED: x -> y (https://c)\nUNRESOLVED: 0\n"
        cb.apply_result("verify", self.ch, message(verified, urls=["https://c"]), r.root, self.state, set(), True, report)
        note = (r.root / self.ch.path).read_text()
        self.assertIn("status: solid", note)
        self.assertIn("corrected", note)
        self.assertIn('"https://a"', note)
        self.assertIn('"https://c"', note)
        self.assertIn(self.ch.path, self.state["done"]["verify"])

    def test_verify_skips_a_hand_edited_draft(self):
        r, report = self.repo, []
        cb.apply_result("draft", self.ch, message(FULL_BODY), r.root, self.state, set(), True, report)
        p = r.root / self.ch.path
        p.write_text(p.read_text() + "\nhuman edit\n")
        self.assertEqual(cb.select("verify", r.root, r.chapters, self.state), [])
        cb.apply_result("verify", self.ch, message(FULL_BODY + "\nUNRESOLVED: 0\n"), r.root, self.state, set(), True, report)
        self.assertIn("human edit", p.read_text())

    def test_verify_keeps_draft_when_correction_shrinks_it(self):
        r, report = self.repo, []
        cb.apply_result("draft", self.ch, message(FULL_BODY), r.root, self.state, set(), True, report)
        before = (r.root / self.ch.path).read_text()
        cb.apply_result("verify", self.ch, message("# Fabrics\n\n## TL;DR\nshort\nUNRESOLVED: 0\n"),
                        r.root, self.state, set(), True, report)
        self.assertEqual((r.root / self.ch.path).read_text(), before)
        self.assertIn("kept the draft", report[-1])

    def test_cards_select_verified_and_hand_written_finished_chapters(self):
        r = self.repo
        r.write(r.chapters[0].path, cb.render_note(r.chapters[0], "solid", "# Hypervisors\n", []))
        r.write(r.chapters[2].path, cb.render_note(r.chapters[2], "draft", "# Dynamo\n", []))
        cb.apply_result("draft", self.ch, message(FULL_BODY), r.root, self.state, set(), True, [])
        self.assertEqual([c.cid for c in cb.select("cards", r.root, r.chapters, self.state)], ["v00c01"])
        self.state["done"]["verify"] = [self.ch.path]
        self.assertEqual([c.cid for c in cb.select("cards", r.root, r.chapters, self.state)], ["v00c01", "v00c02"])
        self.state["done"]["cards"] = [r.chapters[0].path]
        self.assertEqual([c.cid for c in cb.select("cards", r.root, r.chapters, self.state)], ["v00c02"])

    def test_cards_write_tsv_per_volume_and_replace_on_rerun(self):
        r, report = self.repo, []
        msg = message("front one\tback one\nnot a card\nfront two\tback two\n")
        cb.apply_result("cards", self.ch, msg, r.root, self.state, set(), True, report)
        cb.apply_result("cards", self.ch, msg, r.root, self.state, set(), True, report)
        tsv = (r.root / cb.CURRICULUM / "Flashcards" / "Volume_00_Foundations.tsv").read_text().splitlines()
        self.assertEqual(tsv, ["front one\tback one\tChapter_02_Fabrics", "front two\tback two\tChapter_02_Fabrics"])


class FakeBatches:
    """Stands in for `client().messages.batches`; `fail_after` makes a results stream break after N rows."""

    def __init__(self, results: dict, fail_after: dict | None = None):
        self.data, self.fail_after = results, fail_after or {}

    def retrieve(self, batch_id):
        return SimpleNamespace(processing_status="ended")

    def results(self, batch_id):
        for i, r in enumerate(self.data[batch_id]):
            if i == self.fail_after.get(batch_id, -1):
                raise ConnectionError("stream reset")
            yield r


class CollectTest(unittest.TestCase):
    def setUp(self):
        self.repo = Repo(self)
        self.state = cb.load_state(self.repo.root)
        self.a, self.b = self.repo.chapters[0], self.repo.chapters[1]
        for bid, ch in (("b1", self.a), ("b2", self.b)):
            self.state["batches"].append({"id": bid, "phase": "draft", "custom_ids": {f"draft-{ch.cid}": ch.path},
                                          "estimate_high": 1.0, "collected": False})
        self.results = {bid: [{"custom_id": f"draft-{ch.cid}", "result": {"type": "succeeded", "message": message(
            FULL_BODY.replace("# Fabrics", f"# {ch.title}"))}}] for bid, ch in (("b1", self.a), ("b2", self.b))}

    def collect(self, batches: FakeBatches) -> None:
        api = SimpleNamespace(messages=SimpleNamespace(batches=batches))
        orig = cb.client
        cb.client = lambda: api
        try:
            with contextlib.redirect_stdout(io.StringIO()):
                cb.cmd_collect(Namespace(apply=True), self.repo.root, self.repo.chapters, self.state)
        finally:
            cb.client = orig

    def test_interrupted_download_keeps_earlier_batches_and_retries_cleanly(self):
        with self.assertRaises(ConnectionError):
            self.collect(FakeBatches(self.results, fail_after={"b2": 0}))
        saved = cb.load_state(self.repo.root)
        self.assertEqual([b["collected"] for b in saved["batches"]], [True, False])
        self.assertIn(self.a.path, saved["owned"])
        self.assertFalse((self.repo.root / cb.STATE_DIR / "results" / "b2.jsonl").exists())
        self.assertEqual([c.cid for c in cb.select("verify", self.repo.root, self.repo.chapters, saved)], ["v00c01"])

        self.state = saved
        self.collect(FakeBatches(self.results))
        saved = cb.load_state(self.repo.root)
        self.assertEqual([b["collected"] for b in saved["batches"]], [True, True])
        self.assertEqual(cb.note_status(self.repo.root / self.b.path), "draft")
        # Each batch: (1M input * $4 + 0.1M output * $20) * 0.5 + 2 searches * $0.01 = $3.02.
        self.assertAlmostEqual(saved["spent_usd"], 6.04)


class CliTest(unittest.TestCase):
    def test_plan_and_submit_dry_run_write_nothing(self):
        repo = Repo(self)
        for cmd in (["plan"], ["submit", "--phase", "draft"]):
            out = subprocess.run([sys.executable, str(TOOLS / "cloud_batch.py"), "--repo", str(repo.root), *cmd],
                                 capture_output=True, text=True, check=True).stdout
            self.assertTrue(out)
        self.assertEqual(sorted(p.name for p in (repo.root / cb.CURRICULUM).iterdir()), ["STYLE.md", "SUMMARY.md"])

    def test_submit_refuses_over_budget(self):
        repo = Repo(self)
        proc = subprocess.run([sys.executable, str(TOOLS / "cloud_batch.py"), "--repo", str(repo.root),
                               "submit", "--phase", "draft", "--budget", "0.01", "--apply"],
                              capture_output=True, text=True)
        self.assertEqual(proc.returncode, 1)
        self.assertIn("refusing", proc.stdout)


if __name__ == "__main__":
    unittest.main()
