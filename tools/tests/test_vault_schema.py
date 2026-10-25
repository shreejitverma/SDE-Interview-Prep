"""Unit tests for tools/vault_schema.py."""

import unittest
from pathlib import Path
import sys

TOOLS_DIR = Path(__file__).resolve().parent.parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

import vault_schema


class VaultSchemaTest(unittest.TestCase):
    def test_all_tracks_contains_required_tracks(self):
        expected = {"sde", "quant-dev", "quant-research", "low-latency", "ai-eng", "distinguished", "cloud-ai"}
        self.assertTrue(expected.issubset(set(vault_schema.ALL_TRACKS)))

    def test_tracks_mapping_uses_valid_tracks(self):
        all_tracks_set = set(vault_schema.ALL_TRACKS)
        for folder, tracks in vault_schema.TRACKS.items():
            for t in tracks:
                self.assertIn(t, all_tracks_set, f"Folder {folder} has invalid track {t}")

    def test_cloud_ai_folder_registered(self):
        self.assertIn("17-Cloud-AI-Engineering", vault_schema.TRACKS)
        self.assertIn("cloud-ai", vault_schema.TRACKS["17-Cloud-AI-Engineering"])
        self.assertIn("17-Cloud-AI-Engineering", vault_schema.SCOPES)

    def test_shell_folder_registered(self):
        self.assertIn("19-Shell-and-Unix-Engineering", vault_schema.TRACKS)
        self.assertIn("sde", vault_schema.TRACKS["19-Shell-and-Unix-Engineering"])
        self.assertIn("19-Shell-and-Unix-Engineering", vault_schema.SCOPES)
        self.assertIn("low-latency", vault_schema.tracks_for("19-Shell-and-Unix-Engineering/README.md"))

    def test_tracks_for_utility(self):
        self.assertIn("cloud-ai", vault_schema.tracks_for("17-Cloud-AI-Engineering/README.md"))
        self.assertEqual(vault_schema.tracks_for("nonexistent_folder/note.md"), [])


if __name__ == "__main__":
    unittest.main()
