#ifndef SCANNER_H
#define SCANNER_H

// Token types
typedef enum {
    ERROR,
    FLOAT,
    INTEGER,
    PLUS,
    MINUS,
    MULT,
    DIV,
    POW,
    LPAREN,
    RPAREN,
    ID,
    EOL
} TokenType;

static const char *TokenName[] = {
    "ERROR",
    "FLOAT",
    "INTEGER",
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "POW",
    "LPAREN",
    "RPAREN",
    "ID",
    "EOL"
};

// Token structure - with position info
typedef struct {
    TokenType type;
    int position; // start position in expression
    char *literal; // token literal
} Token;

Token *create_token(TokenType type, const char *src,
    int position, int size);
Token *create_eol_token();
void free_token(Token *token);

// Toke node for token list
typedef struct TokenListNode {
    Token *token;
    struct TokenListNode *next;
    struct TokenListNode *prev;
} TokenListNode;

typedef struct TokenList {
    TokenListNode *head;
    TokenListNode *tail;
    int size;
} TokenList;

TokenList *create_list();
TokenListNode *create_list_node(Token *token);
void free_list_node(TokenListNode *node);
void free_list(TokenList *list);
void print_list(TokenList *list);
void append_token(TokenList *list, TokenListNode *node);
TokenListNode *remove_last(TokenList *list);

// Scanner
typedef struct Scanner {
    const char *expression;
    TokenList *token_list;
    int status; // 1 for success, 0 for failure
} Scanner;

Scanner *create_scanner(const char *expression);
void tokonize(Scanner *scanner);
void free_scanner(Scanner *scanner);

#endif