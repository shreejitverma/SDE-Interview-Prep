#!/bin/sh
# make-fixture: build the directory tree the pathname expansion tests glob over.
# Usage: make-fixture.sh EMPTY_DIR
# The names are ASCII only, so the tree is identical on case-insensitive and case-sensitive file
# systems: no two names differ only by case.
set -eu
cd "$1"
mkdir dir dir/sub 'a[b'
: >.hidden
: >a.c
: >b.c
: >b.h
: >./-n
: >'sp ace.c'
: >dir/x.c
: >dir/sub/y.c
: >dir/.dot
: >'a[b/c]d'
: >B1
: >a1
: >'[x]'
: >'^c'
ln -s nowhere dangling
