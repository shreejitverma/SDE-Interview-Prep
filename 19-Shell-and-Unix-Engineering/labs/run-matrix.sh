#!/usr/bin/env bash
# run-matrix.sh: run the lab bats suites once per shell, on this machine or inside the matrix images.
#
# Usage:
#   labs/run-matrix.sh [--list] [LAB...]               every shell found on this machine
#   labs/run-matrix.sh --containers [--list] [LAB...]  every shell in each labs/matrix/*.Dockerfile image
#
#   LAB           a lab directory under labs/ (default: every lab that has a .bats test)
#   --list        print the shells that would run, with their versions, and exit
#   --containers  build each matrix image, then run this script inside it as an unprivileged user
#
# A shell that is not installed is reported as "not installed", never silently dropped.
# Exit status: 0 when every suite passes under every shell, 1 when any fails, 2 on a usage or setup error.
#
# Written for bash 3.2 so it runs with macOS /bin/bash as well as inside the images.

set -u

LABS_DIR=$(cd "$(dirname "$0")" && pwd -P)

# label|family|command: the shells to try, in report order.
# family is what lab_shell_family must report, exported to the tests as LAB_EXPECT_FAMILY;
# it is empty for sh, /bin/sh, and ksh, which are a different shell on each host.
# The command may carry arguments ("busybox ash"); it is split on spaces, as LAB_SHELL is.
CANDIDATES=(
  "sh||sh"
  "sh-system||/bin/sh"
  "bash-system|bash|/bin/bash"
  "bash-path|bash|bash"
  "bash-3.2|bash|/opt/shells/bash-3.2"
  "bash-4.4|bash|/opt/shells/bash-4.4"
  "bash-5.1|bash|/opt/shells/bash-5.1"
  "bash-5.2|bash|/opt/shells/bash-5.2"
  "bash-5.3|bash|/opt/shells/bash-5.3"
  "dash|dash|dash"
  "posh|posh|posh"
  "ksh93|ksh93|ksh93"
  "ksh||ksh"
  "mksh|mksh|mksh"
  "yash|yash|yash"
  "busybox-ash|busybox|busybox ash"
  "zsh|zsh|zsh"
  "zsh-sh|zsh-sh|zsh --emulate sh"
)

die() {
  printf 'run-matrix: %s\n' "$*" >&2
  exit 2
}

usage() {
  sed -n '2,13s/^# \{0,1\}//p' "$0"
}

# resolve PATH: print PATH with symlinks resolved, or PATH unchanged where readlink -f is unavailable.
resolve() {
  readlink -f "$1" 2>/dev/null || printf '%s\n' "$1"
}

# lab_dirs LAB...: print the lab directories to test, one per line.
lab_dirs() {
  local dir name
  if [ "$#" -gt 0 ]; then
    for name in "$@"; do
      [ -d "$LABS_DIR/$name" ] || die "no lab directory named '$name' under $LABS_DIR"
      printf '%s\n' "$LABS_DIR/$name"
    done
    return 0
  fi
  for dir in "$LABS_DIR"/*/; do
    dir=${dir%/}
    case ${dir##*/} in
      lib | matrix) continue ;;
      *) ;;
    esac
    if ls "$dir"/*.bats >/dev/null 2>&1; then
      printf '%s\n' "$dir"
    fi
  done
}

run_containers() {
  local dockerfile image name rc=0 status
  command -v docker >/dev/null 2>&1 || die "docker not found; install Docker or OrbStack, or run without --containers"
  docker info >/dev/null 2>&1 || die "the docker daemon is not running (OrbStack: orb start)"
  for dockerfile in "$LABS_DIR"/matrix/*.Dockerfile; do
    name=$(basename "$dockerfile" .Dockerfile)
    image="vault-shell-matrix:$name"
    printf '== building %s\n' "$image"
    docker build -q -t "$image" -f "$dockerfile" "$LABS_DIR/matrix" >/dev/null \
      || die "docker build failed for $dockerfile"
    docker run --rm -v "$LABS_DIR:/labs:ro" "$image" bash /labs/run-matrix.sh --in-container "$@"
    status=$?
    [ "$status" -eq 0 ] || rc=$status
  done
  return "$rc"
}

main() {
  local containers=0 list=0 in_container=0 arg
  local -a labs=()
  for arg in "$@"; do
    case $arg in
      --containers) containers=1 ;;
      --list) list=1 ;;
      --in-container) in_container=1 ;;
      -h | --help)
        usage
        exit 0
        ;;
      -*) die "unknown option '$arg' (see --help)" ;;
      *) labs+=("$arg") ;;
    esac
  done

  if [ "$containers" -eq 1 ]; then
    local -a pass=()
    [ "$list" -eq 1 ] && pass+=(--list)
    run_containers ${pass[@]+"${pass[@]}"} ${labs[@]+"${labs[@]}"}
    exit $?
  fi

  command -v bats >/dev/null 2>&1 || die "bats not found; brew install bats-core (macOS) or apt-get install bats"
  # shellcheck source=SCRIPTDIR/lib/lab.bash
  . "$LABS_DIR/lib/lab.bash"

  local system os=""
  system=$(uname -srm) || system="unknown system"
  if [ "$in_container" -eq 1 ]; then
    os=$(sed -n 's/^PRETTY_NAME="\{0,1\}\([^"]*\)"\{0,1\}$/\1/p' /etc/os-release) || os=""
    printf '== container %s (%s)\n' "${os:-unknown OS}" "$system"
  else
    printf '== host (%s)\n' "$system"
  fi
  if [ "$in_container" -eq 1 ] && [ -r /etc/shell-matrix-versions ]; then
    sed 's/^/   pkg /' /etc/shell-matrix-versions
  fi

  local -a dirs=() files=()
  local dir found
  # Capture first so a die inside lab_dirs stops the run instead of dying in a subshell.
  found=$(lab_dirs ${labs[@]+"${labs[@]}"}) || exit 2
  while IFS= read -r dir; do
    [ -n "$dir" ] && dirs+=("$dir")
  done <<EOF_DIRS
$found
EOF_DIRS
  [ "${#dirs[@]}" -gt 0 ] || die "no labs with .bats tests under $LABS_DIR"
  for dir in "${dirs[@]}"; do
    files+=("$dir"/*.bats)
  done

  local entry label family cmd first path real key seen=" " version log rc tests failed ran=0 bad=0 missing=0
  local -a failures=()
  log=$(mktemp "${TMPDIR:-/tmp}/run-matrix.XXXXXX") || die "mktemp failed"
  # shellcheck disable=SC2064 # expand $log now; it never changes
  trap "rm -f '$log'" EXIT
  for entry in "${CANDIDATES[@]}"; do
    label=${entry%%|*}
    cmd=${entry#*|}
    family=${cmd%%|*}
    cmd=${cmd#*|}
    first=${cmd%% *}
    if ! command -v "$first" >/dev/null 2>&1; then
      printf '   %-12s %-38s not installed\n' "$label" "-"
      missing=$((missing + 1))
      continue
    fi
    # Skip a shell already run under another label (bash-path is often /bin/bash).
    # A shell invoked as "sh" is keyed apart: bash and zsh change behavior under that name.
    path=$(command -v "$first") || path=$first
    real=$(resolve "$path") || real=$path
    key="$real${cmd#"$first"}"
    if [ "${first##*/}" = sh ]; then
      key="as-sh:$key"
    fi
    case $seen in
      *" $key "*)
        printf '   %-12s %-38s same binary as an earlier entry (%s)\n' "$label" "-" "$real"
        continue
        ;;
      *) seen="$seen$key " ;;
    esac
    version=$(LAB_SHELL=$cmd lab_shell_version)
    if [ "$list" -eq 1 ]; then
      printf '   %-12s %-38s %s\n' "$label" "$version" "$real${cmd#"$first"}"
      continue
    fi
    if LAB_SHELL=$cmd LAB_EXPECT_FAMILY=$family bats --formatter tap "${files[@]}" >"$log" 2>&1; then
      rc=0
    else
      rc=$?
    fi
    tests=$(grep -c '^ok ' "$log")
    failed=$(grep -c '^not ok ' "$log")
    ran=$((ran + 1))
    if [ "$rc" -eq 0 ] && [ "$failed" -eq 0 ]; then
      printf '   %-12s %-38s PASS %3s/%-3s %s\n' "$label" "$version" "$tests" "$tests" "$real"
    else
      bad=$((bad + 1))
      printf '   %-12s %-38s FAIL %3s/%-3s %s\n' "$label" "$version" "$tests" "$((tests + failed))" "$real"
      failures+=("$label")
      sed 's/^/      /' "$log"
    fi
  done

  [ "$list" -eq 1 ] && exit 0
  printf '   %s shells ran, %s failed, %s not installed; labs: %s\n' "$ran" "$bad" "$missing" "${#dirs[@]}"
  [ "$ran" -gt 0 ] || die "no shell from the matrix is installed here"
  if [ "$bad" -gt 0 ]; then
    printf '   failing shells: %s\n' "${failures[*]}"
    exit 1
  fi
  exit 0
}

main "$@"
