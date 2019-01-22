#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "1cc.h"

// function to report error
void error(int i) {
  fprintf(stderr, "予期せぬトークンです: %s\n",
          tokens[i].input);
  exit(1);
}

void error_msg(char *msg, int i) {
  fprintf(stderr, msg, tokens[i].input);
  exit(1);
}

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Vector *new_vector() {
  Vector *vec = malloc(sizeof(Vector));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

void vec_push(Vector *vec, void *elem) {
  if (vec->capacity == vec->len) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}

void expect(int line, int expected, int actual) {
  if (expected == actual)
    return;
  fprintf(stderr, "%d: %d expected, but got %d\n",
          line, expected, actual);
  exit(1);
}

void runtest() {
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++)
    vec_push(vec, (void *)i);

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);

  printf("OK\n");
}

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  if (strcmp(argv[1], "-test") == 0) {
    runtest();
    return 0;
  }

  tokenize(argv[1]);
  program();

  // Output the preface of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologe
  // reserve memory for 26 variables
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    // There should be a result value in Stack.
    // Pop it to avoid Stackoverflow.
    printf("  pop rax\n");
  }

  // epiloge
  // the result of the last expression is in RAX and returned
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
