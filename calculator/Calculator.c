//#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <math.h>
#include "Calculator.h"

#ifndef M_PI
#define M_PI  3.1415926535897932384626434
#endif
#ifndef M_E
#define M_E   2.7182818284590452353602875
#endif
#ifndef M_PHI
#define M_PHI 1.6180339887498948482045868
#endif

double eval(Calculator* calculator, AstNode* ast);
double perform_operation(Calculator* calculator, AstNode* ast);
double function_call(Calculator* calculator, AstNode* ast);
double fetch_constant(Calculator* calculator, AstNode* ast);

Calculator* create_calculator() {
    Calculator* calculator = (Calculator*) malloc(sizeof(Calculator));
    calculator->expression = NULL;
    calculator->status = 1;
    calculator->ans = 0.0;
    calculator->parser = NULL;
    return calculator;
}

void free_calculator(Calculator* calculator) {
    if (calculator) {
        free_parser(calculator->parser);
        free(calculator);
    }
}

void recreate_parser(Calculator* calculator, const char* expression) {
    // Release previous parser
    if (calculator->parser) {
        free_parser(calculator->parser);
        calculator->parser = NULL;
    }
    calculator->status = 1;
    // calculator->ans = 0.0; // keep previous answer
    calculator->expression = expression;
    calculator->parser = create_parser(expression);
    parse(calculator->parser);
}

double calculate(Calculator* calculator) {
    double ans = 0.0;
    if (calculator->parser && calculator->parser->status) {
        ans = eval(calculator, calculator->parser->ast);
        if (calculator->status) 
            calculator->ans= ans; // update
    } else {
        calculator->status = 0;
    } 
    return ans;
}

double eval(Calculator* calculator, AstNode* ast) {
    char* endptr;
    if (ast->token->type == FLOAT) {
        return strtod(ast->token->literal, &endptr);
    } else if (ast->token->type == INTEGER) {
        return (double)strtol(ast->token->literal, &endptr, 10);
    } else if (ast->token->type == ID) {
        if (ast->firstChild) { // function call
            return function_call(calculator, ast);
        } else { // constant
            return fetch_constant(calculator, ast);
        }
    } else {
        return perform_operation(calculator, ast); 
    }
    return 0.0;
}

double perform_operation(Calculator* calculator, AstNode* ast)  {
    double left = eval(calculator, ast->firstChild);
    if (!calculator->status) return 0.0;

    double right = 0.0;
    int is_unary = 1;
    
    if (ast->firstChild->nextSibling) {
        right = eval(calculator, ast->firstChild->nextSibling);
        if (!calculator->status) return 0.0;
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
        case POW: return pow(left, right);
        default: fprintf(stderr, "Unkown operation: %s!\n", 
            ast->token->literal);
            calculator->status = 0;
    }
    return 0.0;
}

double function_call(Calculator* calculator, AstNode* ast) {
    double (*func_ptr)(double);
    if (strcasecmp(ast->token->literal, "sin") == 0) {
        func_ptr = sin;
    } else if (strcasecmp(ast->token->literal, "cos") == 0) {
        func_ptr = cos;
    } else if (strcasecmp(ast->token->literal, "tan") == 0) {
        func_ptr = tan;
    } else if (strcasecmp(ast->token->literal, "asin") == 0) {
        func_ptr = asin;
    } else if (strcasecmp(ast->token->literal, "acos") == 0) {
        func_ptr = acos;
    } else if (strcasecmp(ast->token->literal, "atan") == 0) {
        func_ptr = atan;
    } else if (strcasecmp(ast->token->literal, "sinh") == 0) {
        func_ptr = sinh;
    } else if (strcasecmp(ast->token->literal, "cosh") == 0) {
        func_ptr = cosh;
    } else if (strcasecmp(ast->token->literal, "tanh") == 0) {
        func_ptr = tanh;
    } else if (strcasecmp(ast->token->literal, "asinh") == 0) {
        func_ptr = asinh;
    } else if (strcasecmp(ast->token->literal, "acosh") == 0) {
        func_ptr = acosh;
    } else if (strcasecmp(ast->token->literal, "atanh") == 0) {
        func_ptr = atanh;
    } else if (strcasecmp(ast->token->literal, "exp") == 0) {
        func_ptr = exp;
    } else if (strcasecmp(ast->token->literal, "log") == 0) {
        func_ptr = log;
    } else if (strcasecmp(ast->token->literal, "log10") == 0) {
        func_ptr = log10;
    } else if (strcasecmp(ast->token->literal, "log2") == 0) {
        func_ptr = log2;
    } else if (strcasecmp(ast->token->literal, "sqrt") == 0) {
        func_ptr = sqrt;
    } else if (strcasecmp(ast->token->literal, "cbrt") == 0) {
        func_ptr = cbrt;
    } else if (strcasecmp(ast->token->literal, "ceil") == 0) {
        func_ptr = ceil;
    } else if (strcasecmp(ast->token->literal, "floor") == 0) {
        func_ptr = floor;
    } else if (strcasecmp(ast->token->literal, "fabs") == 0) {
        func_ptr = fabs;
    } else {
        fprintf(stderr, "Unkown function: %s!\n", ast->token->literal);
        calculator->status = 0;
    }

    double arg = eval(calculator, ast->firstChild);
    if (calculator->status) 
        return func_ptr(arg);
    return 0.0;
}

double fetch_constant(Calculator* calculator, AstNode* ast) {
    if (strcasecmp(ast->token->literal, "ans") == 0) {
        return calculator->ans;
    } else if (strcasecmp(ast->token->literal, "pi") == 0) {
        return M_PI;
    } else if (strcasecmp(ast->token->literal, "e") == 0) {
        return M_E;
    } else if (strcasecmp(ast->token->literal, "phi") == 0) {
        return M_PHI;
    } else {
        fprintf(stderr, "Unkown constant name: %s!\n", ast->token->literal);
        calculator->status = 0;
    }
    return 0.0;
}
