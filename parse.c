#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "1cc.h"

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

// type of token
struct Token {
  TokenKind kind;
  Token *next;
  int val;      // number if ty is TK_NUM
  char *str;  // token string (for error message)
  int len;  // length of token
};


Node *stmt();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *term();

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// divide a string pointed by p into tokens and preserve them in tokens
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (!strncmp(p, "==", 2) ||
        !strncmp(p, "!=", 2) ||
        !strncmp(p, "<=", 2) ||
        !strncmp(p, ">=", 2) ) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p++; p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' ||
        *p == '(' || *p == ')' || *p == '<' || *p == '>') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません: %s", p);
    exit(1);
  }

  new_token(TK_EOF, cur, p, 1);
  return head.next;
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *stmt() {
  Node *node = equality();
  return node;
}


Node *equality() {
  Node *node = relational();
  for(;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational(){
  Node *node = add();
  for(;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    if (consume("<="))
      node = new_node(ND_LE, node, add());
    if (consume(">"))
      node = new_node(ND_LT, add(), node);
    if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

Node *term() {
  if (token->kind == TK_NUM)
    return new_node_num(expect_number());
  if (consume("(")) {
    Node *node = add();
    if (consume(")"))
      return node;
    error_at(token->str, "開きカッコに対応する閉じカッコがありません");
  }
  error_at(token->str, "数値でも開きカッコでもないトークンです");
}

Node *unary() {
  if (consume("+"))
    return term();
  else if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), term());
  else
    return term();
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) 
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}
