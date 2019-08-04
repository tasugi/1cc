
typedef struct Token Token;

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ,
} NodeKind;

typedef struct Node Node;

// type of node in ast
typedef struct Node {
  NodeKind kind;
  struct Node *lhs;  // left hand side node
  struct Node *rhs;  // right hand side node
  int val;           // number if ty is ND_NUM
  char name;         // variable name if ty is ND_IDENT
};

Token *tokenize(char *p);
Node *stmt();
void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *token;
