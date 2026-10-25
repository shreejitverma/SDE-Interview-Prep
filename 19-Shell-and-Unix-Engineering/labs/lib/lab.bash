# shellcheck shell=bash
# Shared bats helpers for the shell labs.
# Load from a lab's test file with: load ../lib/lab
#
# LAB_SHELL names the shell under test as a command plus optional arguments,
# for example "dash", "busybox ash", or "zsh --emulate sh"; it defaults to "sh".
# labs/run-matrix.sh sets it once per shell in the matrix.
# It is split on spaces only, so a shell path must not contain a space.

# lab_shell_words: split LAB_SHELL into the array LAB_SHELL_WORDS (no globbing, no quote parsing).
lab_shell_words() {
  IFS=' ' read -r -a LAB_SHELL_WORDS <<<"${LAB_SHELL:-sh}"
}

# run_lab SCRIPT [ARG...]: run a lab script under the shell under test, through bats `run`.
run_lab() {
  lab_shell_words
  run "${LAB_SHELL_WORDS[@]}" "$@"
}

# run_lab_c CODE [NAME [ARG...]]: run inline code under the shell under test, like `sh -c`.
run_lab_c() {
  lab_shell_words
  run "${LAB_SHELL_WORDS[@]}" -c "$@"
}

# lab_shell_family: print the family of the shell under test, detected from the shell itself:
# bash, zsh, zsh-sh (zsh emulating sh), yash, busybox, posh, mksh, ksh93, or the command name
# (for example dash) when the shell sets no identifying variable.
lab_shell_family() {
  local family
  lab_shell_words
  # shellcheck disable=SC2016 # these expansions belong to the shell under test
  family=$("${LAB_SHELL_WORDS[@]}" -c '
    if [ -n "${BASH_VERSION-}" ]; then echo bash
    elif [ -n "${ZSH_VERSION-}" ]; then
      if [ "$(emulate)" = sh ]; then echo zsh-sh; else echo zsh; fi
    elif [ -n "${YASH_VERSION-}" ]; then echo yash
    elif [ -n "${BB_ASH_VERSION-}" ]; then echo busybox
    elif [ -n "${POSH_VERSION-}" ]; then echo posh
    else
      case ${KSH_VERSION-} in
        *MIRBSD*) echo mksh ;;
        *93*) echo ksh93 ;;
        "") echo unknown ;;
        *) echo ksh ;;
      esac
    fi' 2>/dev/null) || family=unknown
  if [ "$family" = unknown ]; then
    family=${LAB_SHELL_WORDS[0]##*/}
  fi
  printf '%s\n' "$family"
}

# lab_shell_version: print the version string the shell under test reports about itself, or "n/a".
lab_shell_version() {
  local version
  lab_shell_words
  # shellcheck disable=SC2016 # these expansions belong to the shell under test
  version=$("${LAB_SHELL_WORDS[@]}" -c '
    printf "%s\n" "${BASH_VERSION:-${ZSH_VERSION:-${YASH_VERSION:-${BB_ASH_VERSION:-${POSH_VERSION:-${KSH_VERSION-}}}}}}"
  ' 2>/dev/null) || version=""
  printf '%s\n' "${version:-n/a}"
}
