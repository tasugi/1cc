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

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

void tokenize();
void program();
void gen(Node *node);

void error(char *fmt, ...);
void error_at(char *loc, char *msg);

extern int pos;
extern Node *code[];
extern Token tokens[];
char *user_input;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void runtest();
