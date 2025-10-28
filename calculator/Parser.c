#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Parser.h"

// Declarations
void advance(Parser* parser);
int expect_token(Parser* parser, TokenType expected);
void report_error(Parser* parser, const char* msg);
AstNode* parse_expr(Parser* parser, int level);
AstNode* parse_term(Parser* parser, int level);
AstNode* parse_power(Parser* parser, int level);
AstNode* parse_unary(Parser* parser, int level);
AstNode* parse_factor(Parser* parser, int level);

#ifdef DEBUG

void debug(const char* fn, Parser* parser, int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    printf("CALL %-14s:", fn);
    if (parser->curr) {
        printf("{type:%-7s, literal:%s}\n", 
            TokenName[parser->curr->token->type],
            parser->curr->token->literal);
    } else {
        printf("NULL\n");
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
    printf("\nScanner status: %d\n\n", parser->scanner->status);
    print_list(parser->scanner->token_list);
    printf("\n");
    #endif

    return parser; 
}

void free_parser(Parser* parser) {
    if (parser) {
        free_scanner(parser->scanner);
        free_ast(parser->ast);
        free(parser);
    }
}

void advance(Parser* parser) {
    if (parser->curr)
        parser->curr = parser->curr->next;
}

// Eat expected token
int expect_token(Parser* parser, TokenType expected) {
    if (parser->curr && parser->curr->token->type == expected) {
        return 1;
    }
    return 0;
}

// Report syntax error
void report_error(Parser* parser, const char* msg) {
    //if (// parser->stop_error_report) return; // already reported
    parser->status = 0;
    if (parser->curr) {
        int position = parser->curr->token->position + 1;
        fprintf(stderr, "Syntax Error: %s at position: %d\n", msg, position);
    }
}

int is_token_in_set(TokenType type, TokenType set[], int size) {
    for (int i = 0; i < size; i++) {
        if (type == set[i])
            return 1;
    }
    return 0;
}

// Sync to synchronization set
void error_recovery(Parser* parser, TokenType sync_set[], int size) {
    parser->status = 0;
    // 垃圾处理：panic-mode 恢复，跳过到同步点（该非终结点的 FLLOW 集）
    while (parser->curr && 
            !is_token_in_set(parser->curr->token->type, 
            sync_set, size)) {
        advance(parser);
    }
}

int expect_tokens(Parser* parser, TokenType set[], int size) {
    if (parser->curr && 
            !is_token_in_set(parser->curr->token->type, set, size)) {
        return 0;
    }
    return 1;
}

// Parser entry
void parse(Parser* parser) {
    if (parser->scanner->status) {
        parser->ast = parse_expr(parser, 0);
        // 最终检查：应该到达输入结束
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

// expr ::= term { ( "+" | "-" ) term }
AstNode* parse_expr(Parser* parser, int level) {
    #ifdef DEBUG
    debug(__func__, parser, level);
    #endif

    TokenType firstset[] = {PLUS, MINUS, LPAREN, ID, INTEGER, FLOAT};
    int firstset_size = sizeof(firstset) / sizeof(firstset[0]);
    TokenType followset[] = {RPAREN, EOL};
    int followset_size = sizeof(followset) / sizeof(followset[0]);

    // 检查当前 token 是否在 FIRST(expr) 中
    if (!expect_tokens(parser, firstset, firstset_size)) {
        // report_error(parser, "Unexpected symbol");
        // error_recovery(parser, firstset, firstset_size);
        // advance(parser);
        return NULL;
    }

    // process first term
    AstNode* left = parse_term(parser, level+1);
    if (!left) return NULL;

    // process ( + | -)  term
    while (parser->curr && (parser->curr->token->type == PLUS || 
            parser->curr->token->type == MINUS)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_term(parser, level+1);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, left);
            add_child(operator, right);
            left = operator; // new left 
        } else {
            report_error(parser, "Expected right operand");
            error_recovery(parser, followset, followset_size);
        }
    }

    // 检查 expr 的结束：当前 token 应该在 FOLLOW(expr) 中
    // if (!expect_tokens(parser, followset, followset_size)) {
    //     report_error(parser, "Unexpected symbol");
    //     error_recovery(parser, followset, followset_size);
    //     // return NULL;
    // }

    return left;
}

// term ::= unary { ( * | / ) unary }
AstNode* parse_term(Parser* parser, int level) {
    #ifdef DEBUG
    debug(__func__, parser, level);
    #endif
    
    TokenType firstset[] = {PLUS, MINUS, LPAREN, ID, INTEGER, FLOAT};
    int firstset_size = sizeof(firstset) / sizeof(firstset[0]);
    TokenType followset[] = {PLUS, MINUS, RPAREN, EOL};
    int followset_size = sizeof(followset) / sizeof(followset[0]);

    if (!expect_tokens(parser, firstset, firstset_size)) {
        // report_error(parser, "Unexpected symbol");
        // error_recovery(parser, firstset, firstset_size);
        // advance(parser);
        return NULL;
    }

    AstNode* left = parse_unary(parser, level+1);
    if (!left) return NULL;

    while (parser->curr && (parser->curr->token->type == MULT || 
            parser->curr->token->type == DIV)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_unary(parser, level+1);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, left);
            add_child(operator, right);
            left = operator; // new left 
        } else {
            report_error(parser, "Expected right operand");
            error_recovery(parser, followset, followset_size);
        }
    }

    // if (!expect_tokens(parser, followset, followset_size)) {
    //     report_error(parser, "Unexpected symbol");
    //     error_recovery(parser, followset, followset_size);
    //     // return NULL;
    // }
    
    return left;
}

// unary ::= ( + | - ) unary | power
AstNode* parse_unary(Parser* parser, int level) {
    #ifdef DEBUG
    debug(__func__, parser, level);
    #endif

    TokenType firstset[] = {PLUS, MINUS, LPAREN, ID, INTEGER, FLOAT};
    int firstset_size = sizeof(firstset) / sizeof(firstset[0]);
    TokenType followset[] = {PLUS, MINUS, MULT, DIV, POW, RPAREN, EOL};
    int followset_size = sizeof(followset) / sizeof(followset[0]);

    if (!expect_tokens(parser, firstset, firstset_size)) {
        // report_error(parser, "Unexpected symbol");
        // error_recovery(parser, firstset, firstset_size);
        // advance(parser);
        return NULL;
    }

    AstNode* left = NULL;

    if (parser->curr && (parser->curr->token->type == PLUS || 
            parser->curr->token->type == MINUS)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_unary(parser, level+1);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, right);
            left = operator;
        } else {
            report_error(parser, "Expected unary operand");
            error_recovery(parser, followset, followset_size);
        }
    } else {
        left = parse_power(parser, level+1);
    }

    // if (!expect_tokens(parser, followset, followset_size)) {
    //     report_error(parser, "Unexpected symbol");
    //     error_recovery(parser, followset, followset_size);
    //     // return NULL;
    // }
    
    return left;
}

// power ::= factor { ^ unary }
AstNode* parse_power(Parser* parser, int level) {
    #ifdef DEBUG
    debug(__func__, parser, level);
    #endif

    TokenType firstset[] = {LPAREN, ID, INTEGER, FLOAT};
    int firstset_size = sizeof(firstset) / sizeof(firstset[0]);
    TokenType followset[] = {PLUS, MINUS, MULT, DIV, POW, RPAREN, EOL};
    int followset_size = sizeof(followset) / sizeof(followset[0]);

    if (!expect_tokens(parser, firstset, firstset_size)) {
        // report_error(parser, "Unexpected symbol");
        // error_recovery(parser, firstset, firstset_size);
        // advance(parser);
        return NULL;
    }

    AstNode* left = parse_factor(parser, level+1);

    if (parser->curr && (parser->curr->token->type == POW)) {
        Token* token = parser->curr->token;
        advance(parser);
        AstNode* right = parse_unary(parser, level+1);
        if (right) {
            AstNode* operator = create_ast_node(token); 
            add_child(operator, left);
            add_child(operator, right);
            left = operator;
        } else {
            report_error(parser, "Expected right operand");
            error_recovery(parser, followset, followset_size);
        }
    }

    // if (!expect_tokens(parser, followset, followset_size)) {
    //     report_error(parser, "Unexpected symbol");
    //     error_recovery(parser, followset, followset_size);
    //     // return NULL;
    // }
    
    return left;
}

// factor ::= ( expr ) | id ( expr ) | id | integer | float
AstNode* parse_factor(Parser* parser, int level) {
    #ifdef DEBUG
    debug(__func__, parser, level);
    #endif

    TokenType firstset[] = {LPAREN, ID, INTEGER, FLOAT};
    int firstset_size = sizeof(firstset) / sizeof(firstset[0]);
    TokenType followset[] = {PLUS, MINUS, MULT, DIV, POW, RPAREN, EOL};
    int followset_size = sizeof(followset) / sizeof(followset[0]);

    if (!expect_tokens(parser, firstset, firstset_size)) {
        // report_error(parser, "Unexpected symbol");
        // error_recovery(parser, firstset, firstset_size);
        // advance(parser);
        return NULL;
    }

    if (!parser->curr) {
        report_error(parser, "Terminated abnormally");
        return NULL;
    }

    Token* token = parser->curr->token;
    AstNode* node = NULL;
    if (token->type == FLOAT) {
        node = create_ast_node(token);
        advance(parser);
        // return node;
    } else if (token->type == INTEGER) {
        node = create_ast_node(token);
        advance(parser);
        // return node;
    } else if (token->type == LPAREN) {
        advance(parser);
        node = parse_expr(parser, level+1);
        if (node) {
            if (expect_token(parser, RPAREN)) {
                advance(parser);
                // return node;
            } else {
                report_error(parser, "Expected ')'");
                error_recovery(parser, followset, followset_size);
                // return node;  // 返回已解析的部分
            }
        }
    } else if (token->type == ID) {
        TokenListNode* next_token = parser->curr->next;

        if (next_token && next_token->token->type == LPAREN) { // function call
            node = create_ast_node(token);
            advance(parser); // function name
            advance(parser); // LPAREN
            AstNode* arg = parse_expr(parser, level+1);
            if (arg) {
                if (expect_token(parser, RPAREN)) {
                    add_child(node, arg);
                    advance(parser);
                    // return node;
                } else {
                    report_error(parser, "Expected ')'");
                    error_recovery(parser, followset, followset_size);
                    // return node;  // 返回已解析的部分
                } 
            }
        } else { // constant
            node = create_ast_node(token);
            advance(parser);
            // return node;
        }
    } else {
        report_error(parser, "Unexpected symbol");
        error_recovery(parser, followset, followset_size);
        // return NULL;
    }

    // if (!expect_tokens(parser, followset, followset_size)) {
    //     report_error(parser, "Unexpected symbol");
    //     error_recovery(parser, followset, followset_size);
    //     // return NULL;
    // }

    return node;
}