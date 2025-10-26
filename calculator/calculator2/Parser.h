#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"

// Abstract Syntax Trees (ASTs) are typically multi-way tree structures, 
// where each node (such as a function definition) may have multiple child 
// nodes (such as argument lists and function bodies). To represent and 
// process ASTs in a binary tree, we can use the Left-Child Right-Sibling 
// representation, a classic method for converting multi-way trees to 
// binary trees.

// Abstract Syntax Tree Node
typedef struct AstNode {
    Token* token;                  // token pointer
    struct AstNode* firstChild;    // left: child
    struct AstNode* nextSibling;   // right: sibling
} AstNode;

// Create a AST node
AstNode* create_ast_node(Token* token);

// Append a child to parent's child list
void add_child(AstNode* parent, AstNode* child);

// Postorder print 
void print_ast(AstNode* root);

// Destroy AST
void free_ast(AstNode* ast);

typedef struct Parser {
    const char* expression;
    Scanner* scanner;
    TokenListNode* curr;  
    AstNode* ast;
    int status; // 1 for success, 0 for failure
} Parser;

Parser* create_parser(const char* expression);
void parse(Parser* parser);
void free_parser(Parser* parser);

#endif 