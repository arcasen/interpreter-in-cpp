#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Parser.h"

// Declarations
void advance(Parser* parser);
int expect(Parser* parser, TokenType expected);
void syntax_error(Parser* parser, const char* msg);
AstNode* parse_expr(Parser* parser);
AstNode* parse_term(Parser* parser);
AstNode* parse_unary(Parser* parser);
AstNode* parse_factor(Parser* parser);

#ifdef DEBUG

void debug(const char* fn, Parser* parser) {
    printf("FUNCTION %-15s:", fn);
    if (parser->curr) {
        printf(" current token: {type:%-8s, literal:%s}\n", 
            TokenName[parser->curr->token->type],
            parser->curr->token->literal);
    } else {
        printf("current token: NULL\n");
    }
}

#endif

// Create a AST node
AstNode* create_ast_node(Token* token) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (node == NULL) {
        printf("Allocation failed!\n");
        return NULL;
    }
    node->token = token;
    node->firstChild = NULL;
    node->nextSibling = NULL;
    return node;
}

// Append a child to parent's child list
void add_child(AstNode* parent, AstNode* child) {
    if (parent == NULL) {
        return;
    }
    
    if (parent->firstChild == NULL) {
        parent->firstChild = child;
    } else {
        // Append the right most
        AstNode* sibling = parent->firstChild;
        while (sibling->nextSibling != NULL) {
            sibling = sibling->nextSibling;
        }
        sibling->nextSibling = child;
    }
}

// Postorder print
void print_ast(AstNode* root) {
    if (root == NULL) {
        return;
    }
    
    // Print all children
    AstNode* child = root->firstChild;
    while (child != NULL) {
        print_ast(child);  // recursively
        child = child->nextSibling;
    }
    printf("%s ", root->token->literal);  // vist parent
}

// Destroy AST
void free_ast(AstNode* ast) {
    if (ast == NULL) {
        return;
    }
    
    // Destroy recursively
    free_ast(ast->firstChild);
    free_ast(ast->nextSibling);
    free(ast);
}

Parser* create_parser(const char* expression) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->expression = expression;
    parser->scanner =  create_scanner(expression);
    parser->ast = NULL;
    parser->status = 1;
    tokonize(parser->scanner);
    parser->curr = parser->scanner->token_list->head;
    return parser; 
}

void parse(Parser* parser) {
    if (parser->scanner->status) {
        parser->ast = parse_expr(parser);
        if (parser->curr && parser->curr->token->type != EOL) {
            syntax_error(parser, "Unexpected symbol");
        }
    } else {
        parser->ast = NULL;
        parser->status = 0; 
    }
    
}

void free_parser(Parser* parser) {
    free_scanner(parser->scanner);
    free_ast(parser->ast);
    free(parser);
}

void advance(Parser* parser) {
    parser->curr = parser->curr->next;
}

// Eat expected token
int expect(Parser* parser, TokenType expected) {
    if (parser->curr && parser->curr->token->type == expected) {
        return 1;
    }
    return 0;
}

// Report syntax error
void syntax_error(Parser* parser, const char* msg) {
    parser->status = 0;
    int position = strlen(parser->expression);
    if (parser->curr) {
        position = parser->curr->token->position + 1;
    }
    fprintf(stderr, "Syntax Error: %s at position: %d\n", msg, position);
    // 垃圾处理：panic-mode 恢复，跳过到同步点（运算符、右括号或 EOF）
    while (parser->curr && 
            parser->curr->token->type != PLUS && 
            parser->curr->token->type != MINUS &&
            parser->curr->token->type != MULT && 
            parser->curr->token->type != DIV &&
            parser->curr->token->type != RPAREN && 
            parser->curr->token->type != EOL) {
        advance(parser);
    }
}

// expr ::= term { ( "+" | "-" ) term }
AstNode* parse_expr(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    AstNode* left = parse_term(parser);
    if (!left) return NULL;

    while (parser->curr && (parser->curr->token->type == PLUS || 
            parser->curr->token->type == MINUS)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_term(parser);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, left);
            add_child(operator, right);
            left = operator; // new left 
        } else {
            syntax_error(parser, "Expected right operand after '+' or '-'");
        }
    }
    
    return left;
}

// term ::= unary { ( * | / ) unary }
AstNode* parse_term(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    AstNode* left = parse_unary(parser);
    if (!left) return NULL;

    while (parser->curr && (parser->curr->token->type == MULT || 
            parser->curr->token->type == DIV)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_unary(parser);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, left);
            add_child(operator, right);
            left = operator; // new left 
        } else {
            syntax_error(parser, "Expected right operand after '*' or '/'");
        }
    }
    
    return left;
}

// unary ::= ( + | - ) unary | factor
AstNode* parse_unary(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    AstNode* left = NULL;

    if (parser->curr && (parser->curr->token->type == PLUS || 
            parser->curr->token->type == MINUS)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_unary(parser);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, right);
            left = operator;
        } else {
            syntax_error(parser, "Expected unary operand after '+' or '-'");
        }
    } else {
        left = parse_factor(parser);
    }
    
    return left;
}

// factor ::= ( expr ) | integer
AstNode* parse_factor(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    if (!parser->curr) {
        syntax_error(parser, "Unexpected EOL");
        return NULL;
    }

    Token* token = parser->curr->token;
    if (token->type == INTEGER) {
        AstNode* node = create_ast_node(token);
        advance(parser);
        return node;
    } else if (token->type == LPAREN) {
        advance(parser);
        AstNode* node = parse_expr(parser);
        if (node && expect(parser, RPAREN)) {
            advance(parser);
            return node;
        } else {
            syntax_error(parser, "Expected ')'");
            return node;  // 返回已解析的部分
        }
        
    } else {
        syntax_error(parser, "Expected integer or '('");
        return NULL;
    }
    return NULL;
}