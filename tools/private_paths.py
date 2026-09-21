"""Paths that hold private job-search data, whatever their content.

These paths are gitignored and symlinked in from the private career-ops repo
(command-center/). Single source of truth for tools/audit_pii.py (which
inventories them) and tools/audit_vault.py (which keeps them out of AUDIT.md).
"""

PRIVATE_LOCATIONS: list[tuple[str, str]] = [
    ("16-Interview-Command-Center/02-Companies/", "company profiles with application status, contacts, and rates"),
    ("16-Interview-Command-Center/03-Pipeline/", "application pipeline (emails, trackers, contacts)"),
    ("16-Interview-Command-Center/04-Retrospectives/", "interview retrospectives"),
    ("16-Interview-Command-Center/05-Behavioral/", "personal STAR stories"),
    ("16-Interview-Command-Center/06-Daily-Log/", "personal daily log"),
]


def is_private(path: str) -> bool:
    return any(path.startswith(prefix) for prefix, _ in PRIVATE_LOCATIONS)


def private_link_prefixes() -> list[str]:
    """Every trailing sub-path of each private location, e.g. "02-Companies/".

    Obsidian resolves [[02-Companies/X]] by path suffix, so a link starting
    with any of these points into private data that exists only locally.
    """
    out: list[str] = []
    for prefix, _ in PRIVATE_LOCATIONS:
        parts = prefix.strip("/").split("/")
        out += ["/".join(parts[i:]) + "/" for i in range(len(parts))]
    return out
