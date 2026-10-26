#!/bin/sh
# shellcheck shell=sh
# proc_probe.sh - Probe /proc filesystem mechanics, virtual metadata, and parsing invariants.
# Part of 19-Shell-and-Unix-Engineering/01-Unix-Process-Model/The-proc-Filesystem

set -eu

action="${1:-help}"
shift || true

case "$action" in
  detect)
    # Mode 1: Detect if /proc is mounted as a virtual filesystem.
    if [ -d /proc ] && [ -e /proc/self ]; then
      printf 'procfs: available\n'
    else
      printf 'procfs: unavailable\n'
    fi
    ;;

  stat_zero)
    # Mode 2: Verify that /proc virtual files report st_size == 0 while streaming bytes.
    if [ ! -d /proc ] || [ ! -e /proc/self ]; then
      printf 'procfs: unavailable\n'
      exit 0
    fi
    # Use ls -ln to inspect directory entry size field (column 5)
    # shellcheck disable=SC2012 # inspecting directory entry size field via ls -ln
    size_field=$(ls -ln "/proc/$$/stat" 2>/dev/null | awk '{print $5}')
    # Stream bytes from seq_file via wc -c
    byte_count=$(wc -c <"/proc/$$/stat" 2>/dev/null | tr -d ' ' || echo 0)
    if [ "$size_field" = "0" ] && [ "$byte_count" -gt 0 ]; then
      printf 'stat_size=0 stream_bytes=nonzero\n'
    else
      printf 'stat_size=%s stream_bytes=%s\n' "$size_field" "$byte_count"
    fi
    ;;

  self_vs_dollar)
    # Mode 3: Dynamic /proc/self resolution versus static /proc/$$ task identification.
    if [ ! -d /proc ] || [ ! -e /proc/self ]; then
      printf 'procfs: unavailable\n'
      exit 0
    fi
    # An external cat reads its own task_struct through /proc/self/comm;
    # /proc/$$/comm reads the parent shell's. The path is explicit because some
    # mksh builds (Alpine R59) have a cat builtin, which would read the shell's own task.
    child_comm=$(/bin/cat /proc/self/comm 2>/dev/null || echo unknown)
    parent_comm=$(cat "/proc/$$/comm" 2>/dev/null || echo unknown)
    printf 'self_comm=%s parent_comm=%s\n' "$child_comm" "$parent_comm"
    ;;

  magic_fd_deleted)
    # Mode 4: Magic symlink behavior for unlinked file descriptors.
    if [ ! -d /proc ] || [ ! -e /proc/self ]; then
      printf 'procfs: unavailable\n'
      exit 0
    fi
    workdir="${1:-}"
    if [ -z "$workdir" ] || [ ! -d "$workdir" ]; then
      printf 'error: missing workdir\n' >&2
      exit 1
    fi
    target="$workdir/magic_test_$$.tmp"
    exec 3>"$target"
    printf 'first_payload\n' >&3
    rm -f "$target"
    # readlink on Linux /proc/$$/fd/3 reveals "(deleted)" suffix
    link_target=$(readlink "/proc/$$/fd/3" 2>/dev/null || echo "")
    case "$link_target" in
      *deleted*) deleted_flag=yes ;;
      *) deleted_flag=no ;;
    esac
    # Magic link reopens struct path directly, permitting further appends
    printf 'second_payload\n' >>"/proc/$$/fd/3"
    content=$(cat "/proc/$$/fd/3" 2>/dev/null || echo "")
    exec 3>&-
    case "$content" in
      *"first_payload"*"second_payload"*) preserved_flag=yes ;;
      *) preserved_flag=no ;;
    esac
    printf 'link_deleted=%s content_preserved=%s\n' "$deleted_flag" "$preserved_flag"
    ;;

  parse_stat_naive)
    # Mode 5: Naive whitespace splitting of /proc/[pid]/stat line.
    stat_line="$1"
    # shellcheck disable=SC2086 # intentional unquoted expansion for naive whitespace split
    set -- $stat_line
    printf 'field2=<%s> field3=<%s> field4=<%s>\n' "${2:-}" "${3:-}" "${4:-}"
    ;;

  parse_stat_robust)
    # Mode 6: Robust boundary parsing of /proc/[pid]/stat handling parentheses and spaces.
    stat_line="$1"
    pid="${stat_line%% \(*}"
    # Extract prefix up to the last closing parenthesis
    prefix="${stat_line%\)*}"
    comm="${prefix#*\(}"
    # Remainder begins after the last closing parenthesis and single space
    remainder="${stat_line##*\)\ }"
    # Split the remainder into fields using read to preserve IFS field splitting across all shells
    read -r state ppid pgrp _ <<EOT
$remainder
EOT
    printf 'pid=<%s> comm=<%s> state=<%s> ppid=<%s> pgrp=<%s>\n' "$pid" "$comm" "$state" "$ppid" "$pgrp"
    ;;

  parse_cmdline_file)
    # Mode 7: Parse null-byte delimited arguments file preserving empty entries.
    cmd_file="$1"
    workdir="${2:-}"
    if [ -z "$workdir" ] || [ ! -d "$workdir" ]; then
      workdir="${cmd_file%/*}"
    fi
    tmp_nl="$workdir/cmdline_nl_$$.tmp"
    tr '\0' '\n' <"$cmd_file" >"$tmp_nl"
    count=0
    args_repr=""
    while IFS= read -r line; do
      count=$((count + 1))
      args_repr="${args_repr}<${line}>"
    done <"$tmp_nl"
    rm -f "$tmp_nl"
    printf '%s count=%d\n' "$args_repr" "$count"
    ;;

  live_cmdline)
    # Mode 8: Parse live /proc/$$/cmdline of the current process.
    if [ ! -d /proc ] || [ ! -e /proc/self ]; then
      printf 'procfs: unavailable\n'
      exit 0
    fi
    # Count null-byte terminators in /proc/$$/cmdline
    arg_count=$(tr -cd '\0' <"/proc/$$/cmdline" | wc -c | tr -d ' ')
    if [ "$arg_count" -gt 0 ]; then
      printf 'live_cmdline_parsed=ok count=%s\n' "$arg_count"
    else
      printf 'live_cmdline_parsed=failed\n'
    fi
    ;;

  *)
    printf 'usage: %s {detect|stat_zero|self_vs_dollar|magic_fd_deleted|parse_stat_naive|parse_stat_robust|parse_cmdline_file|live_cmdline} [args...]\n' "$0" >&2
    exit 1
    ;;
esac
