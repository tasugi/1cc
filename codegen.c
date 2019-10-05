#include "1cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int label = 0;

void gen_lval(Node *node);

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_be_prologue(Node *node) {
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");
}

// gen binary expression
void gen_be(Node *node) {
  switch (node->kind) {
  case ND_ADD:
    gen_be_prologue(node);
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    gen_be_prologue(node);
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    gen_be_prologue(node);
    printf("  imul rdi\n");
    break;
  case ND_DIV:
    gen_be_prologue(node);
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    gen_be_prologue(node);
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    gen_be_prologue(node);
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    gen_be_prologue(node);
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    gen_be_prologue(node);
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  default:
    error("Unsupported node: %s", node->kind);
  }

  printf("  push rax\n");
}

void gen(Node *node) {
  int i = 0;
  int l_label;
  char *arg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    break;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    break;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    break;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    break;
  case ND_IF:
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    l_label = label++;
    if (!node->els) {
      printf("  je .Lend%d\n", l_label);
      gen(node->then);
      printf(".Lend%d:\n", l_label);
      break;
    }
    printf("  je .Lelse%d\n", l_label);
    gen(node->then);
    printf("  jmp .Lend%d\n", l_label);
    printf(".Lelse%d:\n", l_label);
    gen(node->els);
    printf(".Lend%d:\n", l_label);
    break;
  case ND_WHILE:
    l_label = label++;
    printf(".Lbegin%d:\n", l_label);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l_label);
    gen(node->body);
    printf("  jmp .Lbegin%d\n", l_label);
    printf(".Lend%d:\n", l_label);
    break;
  case ND_FOR:
    l_label = label++;
    gen(node->init);
    printf(".Lbegin%d:\n", l_label);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l_label);
    gen(node->body);
    gen(node->inc);
    printf("  jmp .Lbegin%d\n", l_label);
    printf(".Lend%d:\n", l_label);
    break;
  case ND_BLOCK:
    while (node->stmts[i] != NULL) {
      gen(node->stmts[i++]);
      printf("  pop rax\n");
    }
    break;
  case ND_CALL:
    while (node->args[i] != NULL) {
      gen(node->args[i]);
      printf("  pop rax\n");
      printf("  mov %s, rax\n", arg[i++]);
    }
    printf("  call %s\n", node->name);
    printf("  push rax\n");
    break;
  default:
    gen_be(node);
  }
}
