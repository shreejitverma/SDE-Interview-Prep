"""Print a wheel's tags, its WHEEL metadata, the extension filename, and whether this interpreter accepts it.

Usage: inspect_wheel.py <wheel> <extract-dir>
"""

import sys
import zipfile
from pathlib import Path

import packaging.tags
from packaging.utils import parse_wheel_filename


def main(wheel: Path, extract_dir: Path) -> None:
    _, _, _, tags = parse_wheel_filename(wheel.name)
    names = sorted(str(t) for t in tags)
    with zipfile.ZipFile(wheel) as zf:
        zf.extractall(extract_dir)
        wheel_meta = next(n for n in zf.namelist() if n.endswith(".dist-info/WHEEL"))
        meta_tags = sorted(line.split(":", 1)[1].strip()
                           for line in zf.read(wheel_meta).decode().splitlines() if line.startswith("Tag:"))
        modules = sorted(n for n in zf.namelist() if n.endswith(".so"))
    supported = set(packaging.tags.sys_tags())
    accepted = any(t in supported for t in tags)
    print(f"Filename tags: {', '.join(names)}")
    print(f"WHEEL Tag lines: {', '.join(meta_tags)}")
    print(f"Extension module file: {', '.join(Path(m).name for m in modules)}")
    print(f"Accepted by this interpreter's sys_tags(): {'yes' if accepted else 'no'}")


if __name__ == "__main__":
    main(Path(sys.argv[1]), Path(sys.argv[2]))
