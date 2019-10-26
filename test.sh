#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./1cc "$input" > tmp.s
  gcc -o tmp tmp.s tmp-foo.o tmp-plus.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

echo 'int plus(int x, int y) { return x + y; }' | gcc -xc -c -o tmp-plus.o -

try 0 "main() { return 0; }"
try 21 'main() { return 5+20-4; }'
try 47 'main() { return 5+6*7; }'
try 4 "main() { return 8/2; }"
try 15 "main() { return 5 * (9-6); }"
try 10 "main() { return -10 + 20; }"
try 1 "main() { return 2 == 2; }"
try 1 "main() { return 1 != 2; }"
try 0 "main() { return 1 < 1; }"
try 1 "main() { return 1 <= 2; }"
try 1 "main() { return 3 > 2; }"
try 0 "main() { return 3 >= 4; }"
try 3 "main() { a=3; return 3; }"
try 3 "main() { foo = 1; fuga = 2; return foo + fuga; }"
try 3 "main() { return 3; }"
try 2 "main() { if (1) return 2; return 3; }"
try 3 "main() { if (0) return 2; else return 3; }"
try 2 "main() { while (1) return 2; return 3; }"
try 45 "main() { j=0; for(i=0;i<10;i=i+1) {j = j+i;} return j; }"
try 5 "main() { return plus(2, 3); }"
try 2 "one() { return 1; } main() { return one() + 1; }"

echo OK
