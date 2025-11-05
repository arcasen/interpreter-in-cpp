#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define _GNU_SOURCE
#include <math.h>
#ifdef USE_READLINE
#include <readline/history.h>
#include <readline/readline.h>
#else
#include "linenoise.h"
#endif
#include "Calculator.h"
#include "Parser.h"
#include "Scanner.h"
#include "strext.h"

#define HISTFILE ".repl_history" // 历史文件路径
#define MAX_HIST 100 // 最大历史记录数

// trim tailing zeros
void trim_zeros(char *buffer)
{
    char *dot = strchr(buffer, '.');
    if (dot != NULL) {
        char *end = buffer + strlen(buffer) - 1;

        // 从末尾开始删除0
        while (end > dot && *end == '0') {
            *end = '\0';
            end--;
        }

        // 如果小数点后没有数字了，连小数点也删除
        if (end == dot) {
            *dot = '\0';
        }
    }
}

#ifdef USE_READLINE
int main(int argc, char *argv[])
{
    char *line;
    int histfile = 0; // 历史文件扩展名

    // 读取历史文件
    read_history(HISTFILE);
    stifle_history(MAX_HIST); // 限制历史大小

    Calculator *calculator = create_calculator();

    printf("Welcome to the expression calculator!\nEnter 'exit' to exit.\n");

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

        recreate_parser(calculator, line);

        double ans = calculate(calculator);
        char buffer[64];
        if (calculator->status) {
            // printf("Postfix notation: ");
            // print_ast(calculator->parser->ast);
            snprintf(buffer, sizeof(buffer), "%.16g", ans);
            printf("ans: %s\n", buffer);
        }

        free(line);
    }

    // 保存历史到文件
    write_history(HISTFILE);

    free_calculator(calculator);

    return 0;
}
#else
int main(int argc, char *argv[])
{
    char *line;
    int histfile = 0; // 历史文件扩展名

    // 初始化：设置最大长度，加载历史
    linenoiseHistorySetMaxLen(MAX_HIST);
    linenoiseHistoryLoad(HISTFILE);

    Calculator *calculator = create_calculator();

    printf("Welcome to the expression calculator!\nEnter 'exit' to exit.\n");

    while ((line = linenoise("> ")) != NULL) {
        // 添加空行跳过
        if (*line == '\0') {
            free(line);
            continue;
        }

        // 回显用户输入的行
        // printf("Echo: %s\n", line);

        // 添加到历史记录
        if (strlen(line) > 0) {
            linenoiseHistoryAdd(line); // 添加到历史
        }

        // 检查退出命令
        if (strcasecmp(line, "exit") == 0) {
            printf("Bye.\n");
            break;
        }

        recreate_parser(calculator, line);

        double ans = calculate(calculator);
        char buffer[64];
        if (calculator->status) {
            // printf("Postfix notation: ");
            // print_ast(calculator->parser->ast);
            snprintf(buffer, sizeof(buffer), "%.16g", ans);
            printf("ans: %s\n", buffer);
        }

        free(line);
    }

    // 保存历史到文件
    linenoiseHistorySave(HISTFILE);

    free_calculator(calculator);

    return 0;
}
#endif
