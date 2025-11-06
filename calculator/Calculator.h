#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "Parser.h"

typedef struct Calculator {
    const char *expression;
    Parser *parser;
    double ans;
    int status; // 1 for success, 0 for failure
} Calculator;

Calculator *create_calculator();
void recreate_parser(Calculator *calculator, const char *expression);
double calculate(Calculator *calculator);
void free_calculator(Calculator *calculator);

#endif