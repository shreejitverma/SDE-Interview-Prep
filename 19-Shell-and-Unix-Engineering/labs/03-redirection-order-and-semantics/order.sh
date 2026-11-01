#!/bin/sh
# order.sh DIR: run emit.sh under four redirection orders, with DIR (created by the caller) as the
# working directory, and print what each sink received. A newline inside a sink is shown as "|".
here=$(cd "$(dirname "$0")" && pwd) || exit 1
emit=$here/emit.sh
cd "${1:?usage: order.sh DIR}" || exit 1

# sink NAME FILE: print the contents of FILE on one line, "-" for a missing file.
sink() {
  if [ -f "$2" ]; then
    flat=$(tr '\n' '|' <"$2")
    printf '%s=[%s]\n' "$1" "$flat"
  else
    printf '%s=-\n' "$1"
  fi
}

# 1. stdout goes to the file first, then stderr becomes a copy of stdout: both land in the file.
"$emit" >one.file 2>&1
sink one one.file

# 2. stderr becomes a copy of the *old* stdout (the outer sink), then stdout goes to the file.
# shellcheck disable=SC2069 # the order is the point of this line: 2>&1 deliberately comes before >file
{ "$emit" 2>&1 >two.file; } >two.outer
sink two.file two.file
sink two.outer two.outer

# 3. the classic swap through a spare descriptor: stdout and stderr exchange destinations.
{ "$emit" 3>&1 1>&2 2>&3 3>&-; } >three.out 2>three.err
sink three.stdout three.out
sink three.stderr three.err

# 4. two separate opens of one file have two offsets; a duplicate shares one.
"$emit" >four.dup 2>&1
"$emit" >four.open 2>four.open
"$emit" >>four.append 2>>four.append
sink four.dup four.dup
sink four.open four.open
sink four.append four.append
