#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "1cc.h"

// Tokenized tokens are preserved in this array.
// It's assumed that the number of input tokens is less than 100.
Token tokens[100];

// The number of tokens already parsed
int pos = 0;

// Root nodes of each statement are preserved in this array. 
Node *code[100];

// divide a string pointed by p into tokens and preserve them in tokens
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '(' || *p == ')' || *p == ';' || *p == '=') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      tokens[i].ty = TK_IDENT;
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

Node *new_node_ident(char name) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
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
  while (tokens[pos].ty != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node = assign();
  if (!consume(';'))
    error_msg("';'ではないトークンです", pos);
  return node;
}

Node *assign() {
  Node *node = add();
  if (consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *term() {
  if (tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);
  if (tokens[pos].ty == TK_IDENT)
    return new_node_ident(*tokens[pos++].input);
  if (consume('(')) {
    Node *node = add();
    if (consume(')'))
      return node;
    error_msg("開きカッコに対応する閉じカッコがありません: %s\n", pos);
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
