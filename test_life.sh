#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

SRC=""
if [ -f life.c ]; then
	SRC="life.c"
elif [ -f file.c ]; then
	SRC="file.c"
else
	printf '%s\n' "KO: no encuentro life.c ni file.c en $(pwd)"
	exit 1
fi

if ! cc -Wall -Wextra -Werror "$SRC" -o life; then
	printf '%s\n' "KO: no compila ($SRC)."
	printf '%s\n' "Tip: revisa los #include (por ejemplo, file.c incluye \"life.h\")."
	exit 1
fi

pass_count=0
fail_count=0

run_case() {
	name="$1"
	input="$2"
	width="$3"
	height="$4"
	iters="$5"
	expected="$6"

	actual="$(printf "%s\n" "$input" | ./life "$width" "$height" "$iters" | cat -e)"
	if [ "$actual" = "$expected" ]; then
		printf "OK  %s\n" "$name"
		pass_count=$((pass_count + 1))
	else
		printf "KO  %s\n" "$name"
		printf '%s\n' "---- expected ----"
		printf '%s\n' "$expected"
		printf '%s\n' "---- actual ------"
		printf '%s\n' "$actual"
		fail_count=$((fail_count + 1))
	fi
}

run_case "sample 1 (5x5, it=0)" \
	"sdxddssaaww" \
	5 5 0 \
	"     $
 OOO $
 O O $
 OOO $
     $"

run_case "sample 2 (10x6, it=0)" \
	"sdxssdswdxddddsxaadwxwdxwaa" \
	10 6 0 \
	"          $
 O   OOO  $
 O     O  $
 OOO  O   $
  O  OOO  $
          $"

run_case "sample 3 (3x3, it=0)" \
	"dxss" \
	3 3 0 \
	" O $
 O $
 O $"

run_case "sample 4 (3x3, it=1)" \
	"dxss" \
	3 3 1 \
	"   $
OOO$
   $"

run_case "sample 5 (3x3, it=2)" \
	"dxss" \
	3 3 2 \
	" O $
 O $
 O $"

printf "\npassed: %d, failed: %d\n" "$pass_count" "$fail_count"
[ "$fail_count" -eq 0 ]
