#include <stdlib.h>
#include <stdio.h>
#include "Calculator.h"

int eval(AstNode* ast);

Calculator* create_calculator(const char* expression) {
    Calculator* calculator = (Calculator*) malloc(sizeof(Calculator));
    calculator->expression = expression;
    calculator->status = 1;
    calculator->parser = create_parser(expression);
    parse(calculator->parser);
    return calculator;
}

void free_calculator(Calculator* calculator) {
    free_parser(calculator->parser);
    free(calculator);
}

int calculate(Calculator* calculator) {
    if (calculator->parser->status) {
        return eval(calculator->parser->ast);
    }
    calculator->status = 0;
    fprintf(stderr, "Calculation failed!\n");
    return 0;
}

int eval(AstNode* ast) {
    if (ast->token->type == INTEGER) {
        return atoi(ast->token->literal);
    } else {
        int left = eval(ast->firstChild);
        int right = 0;
        int is_unary = 1;
        
        if (ast->firstChild->nextSibling) {
            right = eval(ast->firstChild->nextSibling);
            is_unary = 0;
        }
         
        switch(ast->token->type) {
            case PLUS: 
                if (is_unary) return left;
                return left + right;
            case MINUS: 
                if (is_unary) return - left;
                return left - right;
            case MULT: return left * right;
            case DIV: return left / right;
            default: fprintf(stderr, "Unkown operation: %s!\n", 
                ast->token->literal);
        } 
    }
    return 0;
}