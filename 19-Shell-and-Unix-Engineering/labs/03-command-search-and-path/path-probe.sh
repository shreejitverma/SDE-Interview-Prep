#!/bin/sh
# Probe POSIX XCU 2.9.1.4 and XBD 8.3 Command Search and PATH semantics.
set -u

# Ensure POSIX compliant behavior where relevant
# Create dedicated sandbox within TMPDIR
PROBE_TMP=$(mktemp -d)
trap 'rm -rf "${PROBE_TMP}"' EXIT INT TERM

# 1. Search order: shell function vs external utility via command
mkdir "${PROBE_TMP}/bin1"
printf '#!/bin/sh\nprintf "ext_ran"\n' >"${PROBE_TMP}/bin1/my_probe_cmd"
chmod +x "${PROBE_TMP}/bin1/my_probe_cmd"
PATH="${PROBE_TMP}/bin1:${PATH}"
export PATH

my_probe_cmd() {
  printf "func_ran"
}

res_func=$(my_probe_cmd)
res_cmd=$(command my_probe_cmd)
unset -f my_probe_cmd
printf '1_search_order:   func=%s command=%s\n' "${res_func}" "${res_cmd}"

# 2. Slash bypass: pathname containing slash bypasses function and PATH search
# shellcheck disable=SC2329 # intentionally defined to verify pathname slash bypasses function
my_probe_cmd() {
  printf "func_ran"
}
res_slash=$("${PROBE_TMP}/bin1/my_probe_cmd")
unset -f my_probe_cmd
printf '2_slash_bypass:   slash=%s\n' "${res_slash}"

# 3. Empty PATH element: leading colon represents current working directory
mkdir "${PROBE_TMP}/bin3"
printf '#!/bin/sh\nprintf "ran_cwd"\n' >"${PROBE_TMP}/bin3/probe_cwd"
chmod +x "${PROBE_TMP}/bin3/probe_cwd"
res_empty=$(cd "${PROBE_TMP}/bin3" && PATH=":/usr/bin:/bin" probe_cwd)
printf '3_empty_prefix:   empty_prefix=%s\n' "${res_empty}"

# 4. Hash table caching: remembered location vs earlier PATH insertion
mkdir "${PROBE_TMP}/d_a" "${PROBE_TMP}/d_b"
printf '#!/bin/sh\nprintf "version_b"\n' >"${PROBE_TMP}/d_b/cached_cmd"
chmod +x "${PROBE_TMP}/d_b/cached_cmd"
PATH="${PROBE_TMP}/d_a:${PROBE_TMP}/d_b:${PATH}"
export PATH

# Execute in CURRENT shell process to populate the hash table
cached_cmd >/dev/null

# Insert shadowing binary in d_a (earlier in PATH)
printf '#!/bin/sh\nprintf "version_a"\n' >"${PROBE_TMP}/d_a/cached_cmd"
chmod +x "${PROBE_TMP}/d_a/cached_cmd"

res_before=$(cached_cmd)
hash -r 2>/dev/null || rehash 2>/dev/null || true
res_after=$(cached_cmd)
printf '4_hash_cache:     before=%s after=%s\n' "${res_before}" "${res_after}"

# 5. ENOEXEC fallback and binary heuristic check
mkdir "${PROBE_TMP}/bin5"
printf 'printf "enoexec_ok"\n' >"${PROBE_TMP}/bin5/noshebang"
chmod +x "${PROBE_TMP}/bin5/noshebang"
res_noshebang=$("${PROBE_TMP}/bin5/noshebang")

printf '\0\0\0\0\0\0\0\0' >"${PROBE_TMP}/bin5/badbin"
chmod +x "${PROBE_TMP}/bin5/badbin"
st_badbin=0
# The subshell keeps this stable: a foreground form can crash Debian ksh93u+m 1.0.10.
("${PROBE_TMP}/bin5/badbin") 2>/dev/null || st_badbin=$?
printf '5_enoexec_fall:   script=%s badbin=%s\n' "${res_noshebang}" "${st_badbin}"

# 6. Exit status distinction: 127 (not found) vs 126 (found, not executable)
mkdir "${PROBE_TMP}/bin6"
st_notfound=0
"${PROBE_TMP}/bin6/nonexistent" 2>/dev/null || st_notfound=$?

touch "${PROBE_TMP}/bin6/noexec"
chmod 000 "${PROBE_TMP}/bin6/noexec"
st_noexec=0
"${PROBE_TMP}/bin6/noexec" 2>/dev/null || st_noexec=$?

mkdir "${PROBE_TMP}/bin6/adir"
st_dir=0
"${PROBE_TMP}/bin6/adir" 2>/dev/null || st_dir=$?
printf '6_exit_statuses:  notfound=%s noexec=%s dir=%s\n' "${st_notfound}" "${st_noexec}" "${st_dir}"

# 7. Standard utility recovery via command -p
st_poison=0
(PATH="/nonexistent" ls /dev/null 2>/dev/null) || st_poison=$?
st_recov=0
(PATH="/nonexistent" command -p ls /dev/null >/dev/null 2>&1) || st_recov=$?
printf '7_command_p:      poisoned=%s recovered=%s\n' "${st_poison}" "${st_recov}"
