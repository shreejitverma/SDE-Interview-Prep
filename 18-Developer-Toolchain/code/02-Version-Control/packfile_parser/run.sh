#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TMPDIR_REPO="$(mktemp -d)"
trap 'rm -rf "${TMPDIR_REPO}"' EXIT

export GIT_AUTHOR_NAME="Toolchain Author"
export GIT_AUTHOR_EMAIL="author@example.com"
export GIT_COMMITTER_NAME="Toolchain Committer"
export GIT_COMMITTER_EMAIL="committer@example.com"
export GIT_AUTHOR_DATE="2026-10-25T12:00:00Z"
export GIT_COMMITTER_DATE="2026-10-25T12:00:00Z"
export GIT_CONFIG_GLOBAL=/dev/null
export GIT_CONFIG_SYSTEM=/dev/null
export LC_ALL=C

cd "${TMPDIR_REPO}"
git init -b main -q

# Generate initial file with 500 lines
python3 -c '
with open("code.c", "w") as f:
    for i in range(500):
        f.write(f"Row {i:04d} common content block\n")
'
git add code.c
git commit -m "rev 1" -q

for c in 2 3 4 5 6 7; do
    export GIT_AUTHOR_DATE="2026-10-25T12:0${c}:00Z"
    export GIT_COMMITTER_DATE="2026-10-25T12:0${c}:00Z"
    python3 -c "
with open('code.c', 'r') as f:
    lines = f.readlines()
for j in range(${c} * 20, ${c} * 20 + 20):
    lines[j] = f'Row {j:04d} modified in revision ${c} with extra text\n'
lines.append('New trailing row for revision ${c}\n')
with open('code.c', 'w') as f:
    f.writelines(lines)
"
    git commit -am "rev ${c}" -q
done

git -c pack.threads=1 gc --prune=now -q

python3 "${SCRIPT_DIR}/packfile.py" "${TMPDIR_REPO}"
