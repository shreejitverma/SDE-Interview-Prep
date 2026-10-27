#!/bin/sh
# parse_url.sh - pure POSIX parameter expansion URL parser
# Parsing spawns no process; only the final printf calls can exec
# /usr/bin/printf, in shells where printf is not a builtin (mksh, posh).

set -eu

url="${1:?usage: parse_url.sh <url>}"

scheme="${url%%://*}"
after_scheme="${url#*://}"

authority_and_path="${after_scheme%%\?*}"
query="${after_scheme#*\?}"
[ "${query}" = "${after_scheme}" ] && query=""

authority="${authority_and_path%%/*}"
raw_path="${authority_and_path#*/}"
if [ "${raw_path}" = "${authority_and_path}" ]; then
  url_path="/"
else
  url_path="/${raw_path}"
fi

case ${authority} in
  *:*)
    host="${authority%:*}"
    port="${authority##*:}"
    ;;
  *)
    host="${authority}"
    port=""
    ;;
esac
port="${port:-80}"

dir="${url_path%/*}"
[ -z "${dir}" ] && dir="/"
file="${url_path##*/}"
case ${file} in
  *.*)
    stem="${file%.*}"
    ext="${file##*.}"
    ;;
  *)
    stem="${file}"
    ext=""
    ;;
esac

path_len="${#url_path}"
has_query="${query:+"yes"}"
has_query="${has_query:-"no"}"

printf "SCHEME=%s\n" "${scheme}"
printf "HOST=%s\n" "${host}"
printf "PORT=%s\n" "${port}"
printf "DIR=%s\n" "${dir}"
printf "FILE=%s\n" "${file}"
printf "STEM=%s\n" "${stem}"
printf "EXT=%s\n" "${ext}"
printf "PATH_LEN=%s\n" "${path_len}"
printf "HAS_QUERY=%s\n" "${has_query}"
