// values representing the types of tokens
enum {
  TK_NUM = 256,  // integer token
  TK_IDENT,      // Identifier
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

Node *program();
Node *stmt();
Node *assign();
Node *add();
Node *mul();
Node *term();
