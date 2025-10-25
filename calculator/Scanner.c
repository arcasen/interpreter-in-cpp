#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "Scanner.h"

Token* create_token(TokenType type, const char *src, 
        int position, int size) {
    Token * token = (Token *)malloc(sizeof(Token));
    token->literal = strndup(src+position, size);
    token->type = type;
    token->position = position;
    return token;
}

Token* create_eol_token(int position) {
    Token* token = (Token *)malloc(sizeof(Token));
    token->literal = NULL;
    token->type = EOL;
    token->position = position;
    return token;
}

void free_token(Token* token) {
    free(token->literal);
    free(token);
}

// Create a list
TokenList *create_list() {
    TokenList* list = (TokenList*)malloc(sizeof(TokenList));
    assert(list != NULL);
    list->head = NULL; // Sentinel data is unused
    list->tail = NULL;
    list->size = 0;
    return list;
}

// Create a list node
TokenListNode *create_list_node(Token *token) {
    TokenListNode* node = (TokenListNode*)malloc(sizeof(TokenListNode));
    if (node != NULL) {
        node->token = token;
        node->next = NULL;
    }
    return node;
}

// Destroy a list
void free_list(TokenList *list) {
    if (list == NULL) return;
    TokenListNode *curr = list->head;
    while (curr != NULL) {
        TokenListNode *next = curr->next;
        free_token(curr->token);
        free(curr);
        curr = next;
    }
    free(list);
}

void print_list(TokenList *list) {
    if (list == NULL) return;
    TokenListNode *curr = list->head;
    printf("%d token(s):\n", list->size);
    while (curr != NULL) {
        printf("type: %8s, literal: %s\n", 
            TokenName[curr->token->type], curr->token->literal);
        curr = curr->next;
    }
}

// Append a node to the list
void append_token(TokenList *list, TokenListNode *node) {
    if (list->tail == NULL) { // empty list
        list->head = node;
        list->tail = node; 
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

Scanner* create_scanner(const char* expression) {
    Scanner* scanner = (Scanner*) malloc(sizeof(Scanner));
    scanner->token_list = create_list();
    scanner->expression = expression;
    scanner->status = 1;
    return scanner;
}

void tokonize(Scanner* scanner) {
    const char* expression = scanner->expression;

    int position = 0;
    int expression_len = strlen(expression);

    Token *token = NULL;
    while (position < expression_len) {
        // Skip whitespace
        if (isspace(expression[position])) {
            position++;
            continue;
        }

        token = NULL;
        TokenType type = EOL;
        int start = position;
        char ch = expression[position];
        if (isdigit(ch)) {
            // Parse number literal
            while (isdigit(expression[position])) {
                position++;
            }
            type = INTEGER; 
        } else {
            // Operators and parentheses
            switch (ch) {
                case '+': type = PLUS; break;
                case '-': type = MINUS; break;
                case '*': type = MULT; break;
                case '/': type = DIV; break;
                case '(': type = LPAREN; break;
                case ')': type = RPAREN; break;
                default:
                    scanner->status = 0;
                    fprintf(stderr, 
                        "Unexpected character: '%c' at position: %d.\n", 
                        ch, position+1);      
            }
            // Don't stop the scanner even encountering error
            // and advance past the char 
            position++;  
        }

        // Append to list
        if (type != EOL) {
            token = create_token(type, expression, start, position-start);
            append_token(scanner->token_list, create_list_node(token));
        }       
    }

    // Add EOL token
    token = create_eol_token(position);
    append_token(scanner->token_list, create_list_node(token));
}

void free_scanner(Scanner* scanner) {
    if (scanner == NULL) return;
    free_list(scanner->token_list);
    free(scanner);
}
