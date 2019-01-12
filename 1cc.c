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

enum {
    ND_NUM = 256,  // integer node
};

// type of node in ast
typedef struct Node {
  int ty;            // operator or ND_NUM
  struct Node *lhs;  // left hand side node
  struct Node *rhs;  // right hand side node
  int val;           // number if ty is ND_NUM
} Node;

// Tokenized tokens are preserved in this array.
// It's assumed that the number of input tokens is less than 100.
Token tokens[100];

// The number of tokens already parsed
int pos = 0;

// Root nodes of each statement are preserved in this array. 
Node *code[100];

Node *program();
Node *stmt();
Node *add();
Node *mul();
Node *term();

// divide a string pointed by p into tokens and preserve them in tokens
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '(' || *p == ')' || *p == ';') {
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

void error_msg(char *msg, int i) {
  fprintf(stderr, msg, tokens[i].input);
  exit(1);
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *program() {
  int i = 0;
  while (tokens[pos].ty != TK_EOF) {
    code[i++] = stmt();
  }
  code[i] = NULL;
}

Node *stmt() {
  Node *node = add();
  if (!consume(';')) {
    error_msg("';'ではないトークンです", pos);
  }
  return node;
}

Node *term() {
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }
  if (tokens[pos].ty == '(') {
    pos++;
    Node *node = add();  
    if (tokens[pos].ty != ')') {
      error_msg("開きカッコに対応する閉じカッコがありません: %s\n", pos);
    }
    pos++;
    return node;
  }
  error_msg("数値でも開きカッコでもないトークンです: %s\n", pos);
}

Node *mul() {
  Node *node = term();
  for (;;) {
    if (consume('*'))
      node = new_node('*', node, term());
    else if (consume('/'))
      node = new_node('/', node, term());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume('+')) 
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
    case '+':
      printf("  add rax, rdi\n");
      break;
    case '-':
      printf("  sub rax, rdi\n");
      break;
    case '*':
      printf("  mul rdi\n");
      break;
    case '/':
      printf("  mov rdx, 0\n");
      printf("  div rdi\n");
  }

  printf("  push rax\n");
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

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    // There should be a result value in Stack.
    // Pop it to avoid Stackoverflow.
    printf("  pop rax\n");
  }

  printf("  ret\n");
  return 0;
}
