#include "1cc.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,
  TK_EOF,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
} TokenKind;

// type of token
struct Token {
  TokenKind kind;
  Token *next;
  int val;   // number if ty is TK_NUM
  char *str; // token string (for error message)
  int len;   // length of token
};

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals;

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

void program();
Node *stmt();
Node *expr();
Node *assign();
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

bool is_alnum(char c);

// divide a string pointed by p into tokens and preserve them in tokens
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  locals = calloc(1, sizeof(LVar));

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (!strncmp(p, "==", 2) || !strncmp(p, "!=", 2) || !strncmp(p, "<=", 2) ||
        !strncmp(p, ">=", 2)) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p++;
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=' ||
        *p == '{' || *p == '}' || *p == ',') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      int len = 1;
      while ('a' <= p[len] && p[len] <= 'z')
        len++;
      if (len == 6 && strncmp(p, "return", len) == 0) {
        cur = new_token(TK_RETURN, cur, p, len);
      } else if (len == 2 && strncmp(p, "if", len) == 0) {
        cur = new_token(TK_IF, cur, p, len);
      } else if (len == 4 && strncmp(p, "else", len) == 0) {
        cur = new_token(TK_ELSE, cur, p, len);
      } else if (len == 5 && strncmp(p, "while", len) == 0) {
        cur = new_token(TK_WHILE, cur, p, len);
      } else if (len == 3 && strncmp(p, "for", len) == 0) {
        cur = new_token(TK_FOR, cur, p, len);
      } else {
        cur = new_token(TK_IDENT, cur, p, len);
      }
      p += len;
      continue;
    }

    error("トークナイズできません: %s", p);
    exit(1);
  }

  new_token(TK_EOF, cur, p, 1);
  return head.next;
}

bool is_alnum(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool consume_token(TokenKind kind) {
  if (token->kind != kind)
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return false;
  Token *tok = token;
  token = token->next;
  return tok;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
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

char *expect_ident() {
  if (token->kind != TK_IDENT)
    error_at(token->str, "変数ではありません");
  char *name = token->str;
  token = token->next;
  return name;
}

bool at_eof() { return token->kind == TK_EOF; }

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

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;
  if (consume("{")) {
    node = new_node(ND_BLOCK, NULL, NULL);
    int i = 0;
    while (!consume("}")) {
      node->stmts[i++] = stmt();
    }
    node->stmts[i++] = NULL;
  } else if (consume_token(TK_FOR)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    expect("(");
    node->init = expr();
    expect(";");
    node->cond = expr();
    expect(";");
    node->inc = expr();
    expect(")");
    node->body = stmt();
  } else if (consume_token(TK_WHILE)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    expect("(");
    node->cond = expr();
    expect(")");
    node->body = stmt();
  } else if (consume_token(TK_IF)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume_token(TK_ELSE)) {
      node->els = stmt();
    }
  } else if (consume_token(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
  } else {
    node = expr();
    expect(";");
  }
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();
  for (;;) {
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
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    if (consume("(")) {
      node->kind = ND_CALL;
      strncpy(node->name, tok->str, tok->len);
      if (consume(")")) {
        return node;
      }
      int i=0;
      node->args[i++] = expr();
      while (consume(","))
        node->args[i++] = expr();
      expect(")");
      return node;
    }
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }
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
