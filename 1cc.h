
typedef struct Token Token;

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ,
  ND_NE,  // Not Equal
  ND_LT,  // Less than
  ND_LE,  // Less than or equal
  ND_ASSIGN,  // =
  ND_LVAR,    // local variable
  ND_RETURN,  // return
} NodeKind;

typedef struct Node Node;

// type of node in ast
typedef struct Node {
  NodeKind kind;
  struct Node *lhs;  // left hand side node
  struct Node *rhs;  // right hand side node
  int val;           // number if ty is ND_NUM
  int offset;         // used if kind == ND_LVAR
};

Token *tokenize(char *p);
void program();
void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *token;
extern Node *code[];