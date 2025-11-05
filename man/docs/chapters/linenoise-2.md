# Linenoise 多行输入实现

Linenoise 支持多行输入编辑模式（Multi-line Editing），这允许用户在单次输入中跨多行编辑文本，例如编写代码块、长消息或配置脚本。默认是单行模式（Enter 提交），启用多行后，Enter 键会插入换行符，继续编辑，直到用户按 Ctrl+C（取消）或 Ctrl+D（提交空行或 EOF）结束。

## 启用与基本原理

### API 概述
- `void linenoiseSetMultiLine(int ml);`：设置多行模式。
  - `ml = 1`：启用多行输入。
  - `ml = 0`：禁用（默认单行）。
- 在多行模式下：
  - Enter：插入 `\n`，光标移到新行。
  - Ctrl+C：取消当前编辑，丢弃输入，返回 `NULL`。
  - Ctrl+D：提交当前多行内容（即使为空）。
- 历史记录、自动补全和提示功能在多行模式下同样可用，但补全/提示仅应用于当前行。

多行模式使用 VT100 终端的 ANSI 转义序列处理光标移动和行管理，确保跨行导航（如上下箭头）正常工作。适用于 TTY 终端；非 TTY 下无限制。

## 完整实现示例

以下是一个集成多行输入的 REPL 示例。用户输入 `--multiline` 启用模式；输入多行文本（如代码块），Enter 换行，Ctrl+D 提交。示例还包括历史记录和基本命令处理。

### 代码（main.c）
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linenoise.h"

static void completionCallback(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc, "hello");
        linenoiseAddCompletion(lc, "hello world");
    }
}

static char *hintsCallback(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf, "hello")) {
        *color = 35;  // 品红色
        *bold = 0;
        return " world";
    }
    return NULL;
}

int main(int argc, char **argv) {
    char *line;
    char *prgname = argv[0];
    int multiline = 0;

    // 解析命令行
    while (argc > 1) {
        if (argc >= 2 && !strcmp(argv[1], "--multiline")) {
            multiline = 1;
            argc--; argv++;
        } else {
            fprintf(stderr, "Usage: %s [--multiline]\n", prgname);
            exit(1);
        }
    }

    // 初始化
    linenoiseHistorySetMaxLen(100);
    linenoiseSetCompletionCallback(completionCallback);
    linenoiseSetHintsCallback(hintsCallback);

    if (multiline) {
        linenoiseSetMultiLine(1);  // 启用多行模式
        printf("Multi-line mode enabled. Use Enter for new lines, Ctrl+D to submit, Ctrl+C to cancel.\n");
    }

    // 主循环
    while ((line = linenoise("multi> ")) != NULL) {  // 在多行模式下，linenoise 会阻塞直到提交
        if (line[0] != '\0' && line[0] != '/') {
            printf("You entered (multi-line):\n%s\n", line);  // 打印完整多行内容
            linenoiseHistoryAdd(line);  // 添加整个多行字符串到历史
        } else if (!strncmp(line, "/historylen", 11)) {
            int len = atoi(line + 11);
            linenoiseHistorySetMaxLen(len);
        } else if (!strncmp(line, "/history", 8)) {
            linenoiseHistoryPrint();
        } else if (!strncmp(line, "/quit", 5)) {
            break;
        } else if (line[0] == '/') {
            printf("Unrecognized command: %s\n", line);
        }
        free(line);
    }

    return 0;
}
```

### 编译与运行
```bash
gcc -o multi_example main.c linenoise.c
./multi_example --multiline
```

### 示例交互
```
Multi-line mode enabled. Use Enter for new lines, Ctrl+D to submit, Ctrl+C to cancel.
multi> hello
world
this is
a multi-line
input<Ctrl+D>
You entered (multi-line):
hello
world
this is
a multi-line
input

multi> /quit
```
- 用户输入 `hello` + Enter：插入换行，继续编辑。
- 光标可使用箭头键在多行间移动、删除。
- 提交后，整个内容作为单字符串返回（含 `\n`）。
- 历史中存储完整多行：上下箭头回溯时显示全文本。

## 高级用法与自定义

### 与异步模式结合
多行输入可与异步模式集成：在 `linenoiseEditStart` 后，`linenoiseEditFeed` 会处理跨行输入。启用多行：
```c
// 在异步循环前
linenoiseSetMultiLine(1);

// 在 linenoiseEditStart 中，prompt 保持不变
linenoiseEditStart(&ls, STDIN_FILENO, STDOUT_FILENO, buf, sizeof(buf), "multi> ");
```
超时事件（如之前示例）会隐藏当前多行编辑区域，避免干扰。注意：多行下缓冲区需更大（e.g., 4096 字节）。

### 自定义提交逻辑
默认 Ctrl+D 提交，但你可通过自定义键绑定扩展（Linenoise 无内置键绑定 API，需修改源码或用外部库）。示例：检测输入结束符。
```c
// 处理后检查
if (strstr(line, "\n\n")) {  // 双换行作为结束
    // 自定义逻辑
}
```

### 限制与注意事项
- **终端兼容**：需 ANSI/VT100 支持；Windows 下用 WSL 或 MinTTY。
- **性能**：长多行（>1000 行）可能慢；历史保存时文件会增大。
- **取消处理**：Ctrl+C 返回 `NULL`，循环可 `break` 或继续。
- **禁用动态**：运行时调用 `linenoiseSetMultiLine(0);` 切换回单行。
- **移植**：嵌入式系统需确保 `write(1, ...)` 支持 ANSI。

此实现基于官方示例，适用于脚本编辑器或聊天客户端。若需集成特定项目（如 Redis CLI 风格）或 C++ 版本，提供更多细节！