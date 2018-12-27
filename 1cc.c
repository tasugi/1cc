#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// values representing the types of tokens
enum {
  TK_NUM = 256,  // integer token
  TK_EOF,  // end of file token
};

// type of token
typedef struct {
  int ty;       // type of token
  int val;      // number if ty is TK_NUM
  char *input;  // token string (for error message)
} Token;

// Tokenized tokens are preserved in this array.
// It's assumed that the number of input tokens is less than 100.
Token tokens[100];

// divide a string pointed by p into tokens and preserve them in tokens
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "トークナイズできません: %s\n", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

// function to report error
void error(int i) {
  fprintf(stderr, "予期せぬトークンです: %s\n",
          tokens[i].input);
  exit(1);
}

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // The first token must be number.
  if (tokens[0].ty != TK_NUM) {
    error(0);
  }
  printf("  mov rax, %d\n", tokens[0].val);

  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM) {
        error(i);
      }
      printf("  add rax, %ld\n", tokens[i].val);
      i++;
      continue;
    }
    
    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM) {
        error(i);
      }
      printf("  sub rax, %ld\n", tokens[i].val);
      i++;
      continue;
    }

    error(i);    
  }

  printf("  ret\n");
  return 0;
}
