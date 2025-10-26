#include <stdio.h>
#include <stdlib.h>
//#define _GNU_SOURCE
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "Scanner.h"
#include "Parser.h"
#include "Calculator.h"

#define HISTFILE ".repl_history"  // 历史文件路径
#define MAX_HIST 100              // 最大历史记录数

int main(int argc, char* argv[]) {
    char *line;
    int histfile = 0;  // 历史文件扩展名

    // 读取历史文件
    read_history(HISTFILE);
    stifle_history(MAX_HIST);  // 限制历史大小

    Calculator* calculator = create_calculator();

    printf("Welcome to the Enhanced Calculator! Enter 'exit' to exit.\n");

    while ((line = readline("> ")) != NULL) {
        // 添加空行跳过
        if (*line == '\0') {
            free(line);
            continue;
        }

        // 回显用户输入的行
        // printf("Echo: %s\n", line);

        // 添加到历史记录
        if (strlen(line) > 0) {
            add_history(line);
        }

        // 检查退出命令
        if (strcasecmp(line, "exit") == 0) {
            printf("Bye.\n");
            break;
        }

        initialize(calculator, line);

        double ans = calculate(calculator);
        if (calculator->status) {
            // printf("Postfix notation: ");
            // print_ast(calculator->parser->ast); 
            printf("ans: %lf\n", ans);
        }

        free(line);
    }

    // 保存历史到文件
    write_history(HISTFILE);

    free_calculator(calculator);

    return 0;
}
