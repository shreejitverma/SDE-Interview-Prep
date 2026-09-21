"""Behavior tests for tools/fix_style.py: run `python3 -m unittest discover tools/tests`."""

import sys
import unittest
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import fix_style as fs  # noqa: E402

CASES = [
    ("decorative heading emoji", "## \U0001F4CA OVERALL PROGRESS", "## OVERALL PROGRESS"),
    ("frequency cell", "| 3 | Pool | Hard | \U0001F525\U0001F525 | \u2610 |", "| 3 | Pool | Hard | Med | \u2610 |"),
    ("frequency cell with star", "| 2 | Book | Hard | \U0001F525\U0001F525\U0001F525 \u2B50 | \u2610 |",
     "| 2 | Book | Hard | High (must-know) | \u2610 |"),
    ("premium cell", "|0001|[Two Sum](x)| \U0001F512 | Easy |", "|0001|[Two Sum](x)| Premium | Easy |"),
    ("row without a leading pipe", "0157 | [Read4](u) | [C++](x.cpp) |\U0001F512| Easy |",
     "0157 | [Read4](u) | [C++](x.cpp) |Premium| Easy |"),
    ("premium with a note", "0001 | x | \U0001F512, same as [y](z) |", "0001 | x | Premium, same as [y](z) |"),
    ("premium legend", '* Notes: "\U0001F512" means premium', '* Notes: "Premium" means premium'),
    ("check mark keeps its sign", "- \u2705 Fixes usability", "- \u2713 Fixes usability"),
    ("trailing emoji", "any interview! \U0001F680", "any interview!"),
    ("emoji inside bold", "> **\U0001F525 Godhood Tip**", "> **Godhood Tip**"),
    ("variation-selector arrow", "<a href='#a'>\u2B06\uFE0F Back to Top</a>", "<a href='#a'>Back to Top</a>"),
    ("emoji before a word in a cell", "| T | \U0001F534 Critical | | 5/5 |", "| T | Critical | | 5/5 |"),
    ("spaced em dash", "C++20 \u2014 The Gigantic Leap", "C++20 - The Gigantic Leap"),
    ("tight em dash", "word\u2014word", "word - word"),
    ("attribution in a quote", "> \u2014 Marcus Aurelius", "> \\- Marcus Aurelius"),
    ("leading em dash", "\u2014 attribution", "\\- attribution"),
    ("typographic check mark stays", "- Peering reduces cost \u2713", "- Peering reduces cost \u2713"),
    ("ballot box stays", "\u2610 Profile first", "\u2610 Profile first"),
    ("plain arrow stays", "A \u2192 B", "A \u2192 B"),
]


class FixStyleTest(unittest.TestCase):
    def test_lines(self):
        for name, src, want in CASES:
            with self.subTest(name):
                self.assertEqual(fs.fix_text(src, Counter()), want)

    def test_code_block_keeps_alignment(self):
        src = "```cpp\nint x; // a \u2014 b \u2705\n```"
        self.assertEqual(fs.fix_text(src, Counter()), "```cpp\nint x; // a - b \u2713\n```")

    def test_idempotent(self):
        for _, src, _ in CASES:
            once = fs.fix_text(src, Counter())
            self.assertEqual(fs.fix_text(once, Counter()), once)


if __name__ == "__main__":
    unittest.main()
