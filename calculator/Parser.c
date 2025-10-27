#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Parser.h"

// Declarations
void advance(Parser* parser);
int expect(Parser* parser, TokenType expected);
void report_error(Parser* parser, const char* msg);
AstNode* parse_expr(Parser* parser);
AstNode* parse_term(Parser* parser);
AstNode* parse_power(Parser* parser);
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
    if (ast) {
        // Destroy recursively
        free_ast(ast->firstChild);
        free_ast(ast->nextSibling);
        free(ast);
    }
}

Parser* create_parser(const char* expression) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->expression = expression;
    parser->scanner =  create_scanner(expression);
    parser->ast = NULL;
    parser->status = 1;
    tokonize(parser->scanner);
    parser->curr = parser->scanner->token_list->head;

    #ifdef DEBUG
    printf("\nScanner status: %d\n", parser->scanner->status);
    print_list(parser->scanner->token_list);
    #endif

    return parser; 
}

void parse(Parser* parser) {
    if (parser->scanner->status) {
        parser->ast = parse_expr(parser);
        if (parser->curr && parser->curr->token->type != EOL) {
            report_error(parser, "Unexpected symbol");
        }
    } else {
        parser->ast = NULL;
        parser->status = 0; 
    }

    #ifdef DEBUG
    printf("\nParser status: %d\n", parser->status);
    #endif
}

void free_parser(Parser* parser) {
    if (parser) {
        free_scanner(parser->scanner);
        free_ast(parser->ast);
        free(parser);
    }
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
void report_error(Parser* parser, const char* msg) {
    // if (!parser->status) return; // already reported
    parser->status = 0;
    if (parser->curr && parser->curr->token->type != EOL) {
        int position = parser->curr->token->position + 1;
        fprintf(stderr, 
            "Syntax Error: %s near '%s' at position: %d\n", 
            msg, parser->curr->token->literal, position);
    } else {
        int position = strlen(parser->expression) + 1;
        fprintf(stderr, "Syntax Error: %s at position: %d\n", msg, position);
    }
}

int is_in_set(TokenType type, TokenType followset[], int set_size) {
    for (int i = 0; i < set_size; i++) {
        if (type == followset[i])
            return 1;
    }
    return 0;
}

// Painic mode error recovery
void error_recovery(Parser* parser, TokenType followset[], int set_size) {
    parser->status = 0;
    // 垃圾处理：panic-mode 恢复，跳过到同步点（该非终结点的 FLLOW 集）
    while (parser->curr && 
            !is_in_set(parser->curr->token->type, followset, set_size)) {
        advance(parser);
    }
}

// expr ::= term { ( "+" | "-" ) term }
AstNode* parse_expr(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    TokenType followset[] = {LPAREN, EOL};
    int set_size = sizeof(followset) / sizeof(followset[0]);

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
            report_error(parser, "Expected right operand");
            error_recovery(parser, followset, set_size);
        }
    }

    return left;
}

// term ::= unary { ( * | / ) unary }
AstNode* parse_term(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    TokenType followset[] = {PLUS, MINUS, LPAREN, EOL};
    int set_size = sizeof(followset) / sizeof(followset[0]);

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
            report_error(parser, "Expected right operand");
            error_recovery(parser, followset, set_size);
        }
    }
    
    return left;
}

// unary ::= ( + | - ) unary | power
AstNode* parse_unary(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    TokenType followset[] = {PLUS, MINUS, MULT, DIV, POW, LPAREN, EOL};
    int set_size = sizeof(followset) / sizeof(followset[0]);

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
            report_error(parser, "Expected unary operand");
            error_recovery(parser, followset, set_size);
        }
    } else {
        left = parse_power(parser);
    }
    
    return left;
}

// power ::= factor { ^ unary }
AstNode* parse_power(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    TokenType followset[] = {PLUS, MINUS, MULT, DIV, POW, LPAREN, EOL};
    int set_size = sizeof(followset) / sizeof(followset[0]);

    AstNode* left = parse_factor(parser);

    if (parser->curr && (parser->curr->token->type == POW)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_unary(parser);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, left);
            add_child(operator, right);
            left = operator;
        } else {
            report_error(parser, "Expected right operand");
            error_recovery(parser, followset, set_size);
        }
    }
    
    return left;
}

// factor ::= ( expr ) | id ( expr ) | id | integer | float
AstNode* parse_factor(Parser* parser) {
    #ifdef DEBUG
    debug(__func__, parser);
    #endif

    TokenType followset[] = {PLUS, MINUS, MULT, DIV, POW, LPAREN, EOL};
    int set_size = sizeof(followset) / sizeof(followset[0]);

    if (!parser->curr) {
        report_error(parser, "Terminated abnormally");
        return NULL;
    }

    Token* token = parser->curr->token;
    if (token->type == FLOAT) {
        AstNode* node = create_ast_node(token);
        advance(parser);
        return node;
    } else if (token->type == INTEGER) {
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
            report_error(parser, "Expected ')'");
            error_recovery(parser, followset, set_size);
            return node;  // 返回已解析的部分
        }
    } else if (token->type == ID) {
        TokenListNode* next_token = parser->curr->next;

        if (next_token && next_token->token->type == LPAREN) { // function call
            AstNode* node = create_ast_node(token);
            advance(parser); // function name
            advance(parser); // LPAREN
            AstNode* arg = parse_expr(parser);
            if (arg && expect(parser, RPAREN)) {
                add_child(node, arg);
                advance(parser);
                return node;
            } else {
                report_error(parser, "Expected ')'");
                error_recovery(parser, followset, set_size);
                return node;  // 返回已解析的部分
            } 
        } else { // constant
            AstNode* node = create_ast_node(token);
            advance(parser);
            return node;
        }
    } else {
        report_error(parser, "Expected number or '('");
        error_recovery(parser, followset, set_size);
        return NULL;
    }
    return NULL;
}