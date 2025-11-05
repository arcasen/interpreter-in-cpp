#include "linenoise.h"
#include <stdio.h>
#include <stdlib.h>

#define GREEN "\033[0;32m"
#define NONE "\033[m"
;

int main()
{
    // printf("Print Ctl+D to exit.");
    // lacks a newline(\n), so due to stdout buffering(common in C programs),
    // it doesn't flush/display immediately. Instead, it only appears after
    // the first user input (when linenoise or the next printf triggers a flush).
    printf("Print Ctl+D to exit.\n");

    const char *history_file = ".repl_history"; // 历史文件路径
    // 初始化：设置最大长度，加载历史
    linenoiseHistorySetMaxLen(100);
    if (linenoiseHistoryLoad(history_file) == -1) {
        printf("No previous history file found.\n");
    } else {
        printf("History loaded from %s.\n", history_file);
    }

    char *line;
    while ((line = linenoise(GREEN "hello > " NONE)) != NULL) {
        printf("You wrote: %s\n", line);
        if (line[0] != '\0') {
            linenoiseHistoryAdd(line); // 添加到历史
        }
        free(line); // 或使用 linenoiseFree(line)
    }

    // 退出前保存历史
    if (linenoiseHistorySave(history_file) == 0) {
        printf("History saved to %s.\n", history_file);
    } else {
        perror("Failed to save history");
    }

    return 0;
}