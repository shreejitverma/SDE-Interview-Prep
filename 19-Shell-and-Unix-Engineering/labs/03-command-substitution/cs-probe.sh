#!/bin/sh
# cs-probe: two measurements about command substitution that need a real process tree.
# Usage: cs-probe.sh holder DIR hold|release   - does $(...) wait for a background job?
#        cs-probe.sh big BYTES                 - how does a huge result behave?
mode=${1:?mode}
case $mode in
  holder)
    dir=${2:?dir}
    variant=${3:?variant}
    # shellcheck disable=SC2016 # the markers expand in the job's own shell
    case $variant in
      hold)
        # The job inherits the substitution's pipe as its stdout and stderr; it sleeps one second,
        # then writes a marker file. No timing is involved: end of file on the pipe comes only
        # after the job has written the marker and exited.
        job='/bin/sleep 1; echo done >"$1/late"'
        x=$(
          /bin/sh -c "$job" sh "$dir" &
          echo hi
        )
        ;;
      release)
        # The same kind of job with stdout and stderr redirected away holds no copy of the pipe.
        # It writes the marker only after this script creates DIR/go, or after 30 polls of 0.1 s
        # (at least 3 s), so a shell that returns at once sees no marker with a margin of at
        # least 3 s, and a shell that waits for the job still finishes.
        job='i=0
          while [ ! -e "$1/go" ] && [ "$i" -lt 30 ]; do /bin/sleep 0.1; i=$((i + 1)); done
          echo done >"$1/late"'
        x=$(
          /bin/sh -c "$job" sh "$dir" >/dev/null 2>&1 &
          echo hi
        )
        ;;
      *) exit 2 ;;
    esac
    if [ -e "$dir/late" ]; then late=yes; else late=no; fi
    : >"$dir/go"
    printf 'x=%s late=%s\n' "$x" "$late"
    # Do not leave the job behind: wait up to 10 s for its marker file.
    i=0
    while [ ! -e "$dir/late" ] && [ "$i" -lt 100 ]; do
      /bin/sleep 0.1
      i=$((i + 1))
    done
    [ -e "$dir/late" ]
    ;;
  big)
    n=${2:?bytes}
    x=$(head -c "$n" /dev/zero | tr '\0' x)
    # A here-document is written by the shell itself, so no argv limit applies: n bytes plus the newline.
    bytes=$(
      wc -c <<EOF
$x
EOF
    )
    printf 'chars=%d heredoc_bytes=%d\n' "${#x}" "$((bytes - 1))"
    # An external command receives the value as one argv string, which the kernel limits.
    if /bin/echo "$x" >/dev/null 2>&1; then echo external=ok; else echo external=fail; fi
    ;;
  *) exit 2 ;;
esac
