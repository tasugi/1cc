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

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
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
