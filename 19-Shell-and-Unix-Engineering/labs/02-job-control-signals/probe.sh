#!/bin/sh
# probe.sh MODE: start inner.sh MODE under the shell named by LAB_SHELL.
# The pty modes give the shell a fresh pseudo-terminal as its controlling terminal and type at it;
# the other modes only start the shell with default signal dispositions and an empty mask.
# Needs JCTL (the compiled jctl.c) and LOGDIR (an empty scratch directory) in the environment.
set -u
mode=${1:?usage: probe.sh MODE}
: "${JCTL:?}" "${LOGDIR:?}"
here=$(dirname "$0")
shell=${LAB_SHELL:-sh}
# A zsh or mksh script that only runs `set -m` makes a process group per job but never hands the terminal
# to a foreground job; only an interactive shell does (both tie the terminal hand-off to the interactive flag).
# `-f` keeps zsh's user startup files out of the way; an interactive mksh reads only $ENV, which is unset.
zsh_extra=
pty_extra=
if [ -n "${ZSH_VERSION-}" ]; then
  zsh_extra=" -f -i"
  pty_extra=$zsh_extra
fi
# The modes without a terminal must not turn job control on, so only the pty modes get mksh's -i.
case ${KSH_VERSION-} in
  *MIRBSD*) pty_extra=" -i" ;;
  *) ;;
esac
inner="$here/inner.sh"

case $mode in
  keys | isig_off)
    exec "$JCTL" pty "$LOGDIR" --send 'bg.ready=' --send 'fg.ready=\003' --send 'fg.ready=\034' \
      --send 'fg.ready=\032' --shell "$shell$pty_extra" -- "$inner" "$mode"
    ;;
  intr_remap)
    exec "$JCTL" pty "$LOGDIR" --send 'bg.ready=' --send 'fg.ready=\003' --send 'fg.ready=\030' \
      --shell "$shell$pty_extra" -- "$inner" "$mode"
    ;;
  hup)
    exec "$JCTL" pty "$LOGDIR" --hangup fg.ready --shell "$shell$pty_extra" -- "$inner" "$mode"
    ;;
  handoff)
    # a plain script: no interactive flag, so this shows what `set -m` alone does
    exec "$JCTL" pty "$LOGDIR" --shell "$shell" -- "$inner" handoff
    ;;
  handoff_i)
    # the same probe with the shell interactive: zsh and mksh get their flags, ksh93 needs -i
    handoff_extra=$pty_extra
    if [ -z "$handoff_extra" ]; then
      handoff_extra=" -i"
    fi
    exec "$JCTL" pty "$LOGDIR" --shell "$shell$handoff_extra" -- "$inner" handoff
    ;;
  tin | tin_ign | tin_blk | tout | tout_off | tout_ign | tset | jc_disp)
    exec "$JCTL" pty "$LOGDIR" --shell "$shell$pty_extra" -- "$inner" "$mode"
    ;;
  async_disp)
    exec "$JCTL" clean --shell "$shell$zsh_extra" -- "$inner" "$mode"
    ;;
  stop_cont | cont_ign | tstp_ign)
    # A detached suite leads an orphaned process group, where SIGTSTP is discarded; run in a fresh group.
    exec "$JCTL" clean --own-group --shell "$shell$zsh_extra" -- "$inner" "$mode"
    ;;
  *)
    echo "probe.sh: unknown mode $mode" >&2
    exit 2
    ;;
esac
