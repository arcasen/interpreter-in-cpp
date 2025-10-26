#include <stdio.h>
#include "Scanner.h"
#include "Parser.h"
#include "Calculator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"expression\"\n", argv[0]);
        fprintf(stderr, "Example: %s 2+3*45-(6/7)\n", argv[0]);
        return 1;
    }

    printf("Expression: %s\n", argv[1]);

    Calculator* calculator = create_calculator(argv[1]);

    #ifdef DEBUG
    printf("\nScanner status: %d\n", calculator->parser->scanner->status);
    print_list(calculator->parser->scanner->token_list);
    printf("\nParser status: %d\n", calculator->parser->status);
    #endif

    double ans = calculate(calculator);
    if (calculator->status) {
        printf("Postfix notation: ");
        print_ast(calculator->parser->ast); 
        printf("\nAns: %lf\n", ans);
    }
    free_calculator(calculator);
}