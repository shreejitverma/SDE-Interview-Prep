#!/bin/sh
# inner.sh MODE: the code the shell under test runs; probe.sh starts it.
# Needs JCTL (the compiled jctl.c) and LOGDIR in the environment.
set -u
mode=${1:?usage: inner.sh MODE}

log() {
  printf '%s\n' "$*" >>"$LOGDIR/inner.log"
}

# wait_state PID STATE SECONDS: succeed once the process is in STATE (R, S, T, Z), fail after SECONDS.
wait_state() {
  ws_tries=$(($3 * 10))
  while [ "$ws_tries" -gt 0 ]; do
    ws_state=$("$JCTL" state "$1")
    if [ "$ws_state" = "$2" ]; then
      return 0
    fi
    "$JCTL" msleep 100
    ws_tries=$((ws_tries - 1))
  done
  return 1
}

# wait_done PID SECONDS: succeed once the process has exited (a zombie counts), fail after SECONDS.
wait_done() {
  wd_tries=$(($2 * 10))
  while [ "$wd_tries" -gt 0 ]; do
    wd_state=$("$JCTL" state "$1")
    case $wd_state in
      Z | \?) return 0 ;;
      *) ;;
    esac
    "$JCTL" msleep 100
    wd_tries=$((wd_tries - 1))
  done
  return 1
}

# wait_size_gt FILE BYTES SECONDS: succeed once FILE is larger than BYTES, fail after SECONDS.
wait_size_gt() {
  wg_tries=$(($3 * 10))
  while [ "$wg_tries" -gt 0 ]; do
    wg_size=$("$JCTL" size "$1")
    if [ "$wg_size" -gt "$2" ]; then
      return 0
    fi
    "$JCTL" msleep 100
    wg_tries=$((wg_tries - 1))
  done
  return 1
}

case $mode in
  keys | isig_off | intr_remap | hup)
    set -m
    # A shell that shares the foreground group with its job (ksh93u+) receives the keys too.
    # An interactive zsh ignores them itself and refuses the trap.
    if [ -z "${ZSH_VERSION-}" ]; then
      trap : INT QUIT TSTP
    fi
    case $mode in
      isig_off) stty -isig ;;
      intr_remap) stty intr '^X' quit '^C' ;;
      *) ;;
    esac
    if [ "$mode" != hup ]; then
      "$JCTL" rec "$LOGDIR" bg bg &
    fi
    "$JCTL" rec "$LOGDIR" fg fg
    wait
    ;;
  handoff)
    set -m
    "$JCTL" handoff "$$" >"$LOGDIR/handoff.out"
    ;;
  jc_disp)
    set -m
    "$JCTL" disp jc_async >"$LOGDIR/jc_disp.out" &
    wait
    ;;
  tin | tin_ign | tin_blk)
    set -m
    case $mode in
      tin) rmode=dfl ;;
      tin_ign) rmode=ign ;;
      *) rmode=blk ;;
    esac
    "$JCTL" ttyread "$LOGDIR/tin.log" "$rmode" &
    pid=$!
    if [ "$mode" = tin ]; then
      if wait_state "$pid" T 8; then log tin_state=T; else log tin_state=other; fi
      # The kernel can show T before the shell has reaped the stop, so a single `jobs` races under load.
      # Poll for up to 3 s until the shell reports the stop; a shell that never does (Apple bash 3.2) keeps its last answer.
      tries=0
      while :; do
        jobs >"$LOGDIR/jobs.out" 2>&1
        if grep -qiE 'stopped|suspended' "$LOGDIR/jobs.out"; then break; fi
        tries=$((tries + 1))
        if [ "$tries" -ge 30 ]; then break; fi
        "$JCTL" msleep 100
      done
      { kill -9 "$pid"; } 2>/dev/null
    else
      wait_done "$pid" 8 || log not_finished
    fi
    wait "$pid" 2>/dev/null
    ;;
  tout | tout_off | tout_ign)
    set -m
    if [ "$mode" != tout_off ]; then
      stty tostop
    fi
    case $mode in
      tout_ign) wmode=ign ;;
      *) wmode=dfl ;;
    esac
    "$JCTL" ttywrite "$LOGDIR/tout.log" "$wmode" &
    pid=$!
    if [ "$mode" = tout ]; then
      if wait_state "$pid" T 8; then log tout_state=T; else log tout_state=other; fi
      { kill -9 "$pid"; } 2>/dev/null
    else
      wait_done "$pid" 8 || log not_finished
    fi
    wait "$pid" 2>/dev/null
    ;;
  tset)
    set -m
    "$JCTL" ttywrite "$LOGDIR/tset.log" tcset &
    pid=$!
    if wait_state "$pid" T 8; then log tset_state=T; else log tset_state=other; fi
    { kill -9 "$pid"; } 2>/dev/null
    wait "$pid" 2>/dev/null
    ;;
  async_disp)
    "$JCTL" disp fg >"$LOGDIR/disp.out"
    "$JCTL" disp async >>"$LOGDIR/disp.out" &
    wait
    ;;
  stop_cont | cont_ign | tstp_ign)
    case $mode in
      stop_cont) smode=none ;;
      cont_ign) smode=igncont ;;
      *) smode=igntstp ;;
    esac
    spinlog="$LOGDIR/spin.log"
    "$JCTL" spin "$spinlog" "$smode" &
    pid=$!
    if ! wait_size_gt "$spinlog" 0 5; then
      log no_ticks
    fi
    kill -s TSTP "$pid"
    if [ "$mode" = tstp_ign ]; then
      if wait_state "$pid" T 2; then log tstp_stopped=yes; else log tstp_stopped=no; fi
      before=$("$JCTL" size "$spinlog")
      "$JCTL" msleep 1000
      after=$("$JCTL" size "$spinlog")
      if [ "$after" -gt "$before" ]; then log tstp_ticking=yes; else log tstp_ticking=no; fi
      kill -s STOP "$pid"
      if wait_state "$pid" T 5; then log stop_stopped=yes; else log stop_stopped=no; fi
    else
      if wait_state "$pid" T 5; then log stopped=yes; else log stopped=no; fi
      before=$("$JCTL" size "$spinlog")
      "$JCTL" msleep 1000
      after=$("$JCTL" size "$spinlog")
      if [ "$after" -eq "$before" ]; then log frozen=yes; else log frozen=no; fi
      kill -s CONT "$pid"
      if wait_size_gt "$spinlog" "$after" 5; then log resumed=yes; else log resumed=no; fi
    fi
    { kill -9 "$pid"; } 2>/dev/null
    { wait "$pid"; } 2>/dev/null
    ;;
  *)
    echo "inner.sh: unknown mode $mode" >&2
    exit 2
    ;;
esac
exit 0
