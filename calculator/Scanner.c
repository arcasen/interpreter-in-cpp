// 引入乘法运算后, 由于在解析 -3^2 这样的式子是 - 不能与 3 结合, 不能读出 -3 
// 在文法中去掉正负号的处理 (sign)
// 把正负号当做一元运算处理

#define _GNU_SOURCE // Resolve 'strndup' in GCC
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
    if (token)
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
        node->prev = NULL;
    }
    return node;
}

void free_list_node(TokenListNode* node) {
    if (node) 
        free_token(node->token);
    free(node);
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
        node->prev = list->tail;
        list->tail = node;
    }
    list->size++;
}

TokenListNode* remove_last(TokenList* list) {
    if (list->tail == NULL) 
        return NULL;
    TokenListNode* temp = NULL;
    if (list->head == list->tail) { // only one node
        temp = list->head;
        list->head = NULL;
        list->tail = NULL;
    } else {
        temp = list->tail;
        list->tail = temp->prev;
        list->tail->next = NULL; 
    }
    list->size--; 
    return temp;
}

Scanner* create_scanner(const char* expression) {
    Scanner* scanner = (Scanner*) malloc(sizeof(Scanner));
    scanner->token_list = create_list();
    scanner->expression = expression;
    scanner->status = 1;
    return scanner;
}

// 当新增加数字到 Token 链表时, 处理是否存在符号 
// void process_sign(Scanner* scanner) {
//     // printf("FUNCTION %-15s: current: %s\n", __func__,
//     //     scanner->token_list->tail->token->literal);
//     TokenListNode* last1st = NULL;
//     TokenListNode* last2nd = NULL;
//     TokenListNode* last3rd = NULL;
//     last1st = scanner->token_list->tail; // new INTEGER token
//     last2nd = last1st->prev;
    
//     if (!last2nd || (last2nd->token->type != PLUS && 
//             last2nd->token->type != MINUS)) 
//         return;
    
//     last3rd = last2nd->prev;
//     if (last3rd) {
//         Token* t = last3rd->token;
//         if (!(t->type == LPAREN || t->type == PLUS ||
//                 t->type == MINUS || t->type == MULT || 
//                 t->type == DIV)) {
//             return;
//         }
//     }
    
//     //创建新 INTEGER Token, 并删除原来的最后两个 Token
//     int position = last2nd->token->position;
//     int size = strlen(last1st->token->literal) +
//         strlen(last2nd->token->literal);
    
//     int type = last1st->token->type;
//     Token* token = create_token(type, scanner->expression, position, size);
//     remove_last(scanner->token_list); 
//     free_list_node(last1st);
//     remove_last(scanner->token_list);
//     free_list_node(last2nd);
//     append_token(scanner->token_list, create_list_node(token));
// }

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
        if (isalpha(ch)) {
            // Parse identifier literal
            while (isalpha(expression[position]) || 
                    isdigit(expression[position])) {
                position++;
            }
            type = ID; 
        } else if (isdigit(ch)||ch == '.') {
            // Parse number literal
            const char* str = expression+position;
            char *endptr;
            strtod(str, &endptr);
            type = INTEGER; 
            for (const char *p = str; p < endptr; p++) {
                if (*p == '.' || *p == 'e' || *p == 'E') {
                    type = FLOAT;
                    break;
                }
            }
            position = (endptr - str) + start;
        }else {
            // Operators and parentheses
            switch (ch) {
                case '+': type = PLUS; break;
                case '-': type = MINUS; break;
                case '*': type = MULT; break;
                case '/': type = DIV; break;
                case '^': type = POW; break;
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
            // if (type == FLOAT || type == INTEGER) { // 处理是否存在符号
            //     process_sign(scanner);
            // } 
        }       
    }

    // Add EOL token
    token = create_eol_token(position);
    append_token(scanner->token_list, create_list_node(token));
}

void free_scanner(Scanner* scanner) {
    if (scanner)
        free_list(scanner->token_list);
    free(scanner);
}