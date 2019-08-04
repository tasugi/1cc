typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

// type of token
struct Token {
  TokenKind kind;
  Token *next;
  int val;      // number if ty is TK_NUM
  char *input;  // token string (for error message)
};

enum {
    ND_NUM = 256,  // integer node
    ND_IDENT,      // identifier node
};

// type of node in ast
typedef struct Node {
  int ty;            // operator or ND_NUM
  struct Node *lhs;  // left hand side node
  struct Node *rhs;  // right hand side node
  int val;           // number if ty is ND_NUM
  char name;         // variable name if ty is ND_IDENT
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Token *tokenize(char *p);
Node *stmt();
void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *loc, char *msg);

Token *token;
char *user_input;
