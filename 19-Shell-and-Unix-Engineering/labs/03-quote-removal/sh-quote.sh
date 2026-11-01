#!/bin/sh
# sh-quote.sh: define sh_quote, which prints STRING single-quoted so that one more pass of
# parsing (eval, sh -c, a generated script) gives STRING back byte for byte.
# Use it by sourcing: . ./sh-quote.sh
# The only character that cannot appear inside single quotes is the single quote itself,
# so each one becomes '\'' : close the quote, a quoted quote, reopen the quote.
# Usage: sh_quote STRING
# Example: sh_quote "it's"  ->  'it'\''s'
sh_quote() {
  _sq_in=$1
  _sq_out=\'
  while :; do
    case $_sq_in in
      *\'*)
        _sq_out=$_sq_out${_sq_in%%\'*}"'\\''"
        _sq_in=${_sq_in#*\'}
        ;;
      *) break ;;
    esac
  done
  printf '%s\n' "$_sq_out$_sq_in'"
}
