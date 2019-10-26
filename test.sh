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

try 0 "return 0;"
try 21 'return 5+20-4;'
try 47 'return 5+6*7;'
try 4 "return 8/2;"
try 15 "return 5 * (9-6);"
try 10 "return -10 + 20;"
try 1 "return 2 == 2;"
try 1 "return 1 != 2;"
try 0 "return 1 < 1;"
try 1 "return 1 <= 2;"
try 1 "return 3 > 2;"
try 0 "return 3 >= 4;"
try 3 "a=3; return 3;"
try 3 "foo = 1; fuga = 2; return foo + fuga;"
try 3 "return 3;"
try 2 "if (1) return 2; return 3;"
try 3 "if (0) return 2; else return 3;"
try 2 "while (1) return 2; return 3;"
try 45 "j=0; for(i=0;i<10;i=i+1) {j = j+i;} return j;"
try 5 "return plus(2, 3);"

echo OK
