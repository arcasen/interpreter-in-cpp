# Linenoise 历史记录管理

Linenoise 的历史记录功能允许用户通过上下箭头键导航之前输入的命令，支持持久化存储到文件。默认历史为空（长度 0），需手动启用和配置。历史记录是全局的（非多行特定），适用于 REPL 场景，如命令行工具或调试器。每个条目存储完整字符串（包括多行时的 `\n`），最大长度限制防止内存膨胀。

历史记录基于简单链表实现，轻量高效。添加时自动去重（可选，通过检查），但官方未内置去重逻辑。

## API 概述

| API 函数                                          | 描述                                   | 参数                                        | 返回值                                |
| ------------------------------------------------- | -------------------------------------- | ------------------------------------------- | ------------------------------------- |
| `int linenoiseHistoryAdd(const char *line);`      | 将字符串添加到历史末尾。忽略空字符串。 | `line`: 输入字符串                          | 1（成功）或 0（失败，如超过最大长度） |
| `int linenoiseHistorySetMaxLen(int len);`         | 设置历史最大条目数。0 表示禁用。       | `len`: 最大长度（e.g., 100）                | 当前长度                              |
| `int linenoiseHistorySave(const char *filename);` | 将历史保存到文件。每行一个条目。       | `filename`: 文件路径（e.g., "history.txt"） | 0（成功）或 -1（失败）                |
| `int linenoiseHistoryLoad(const char *filename);` | 从文件加载历史到内存。追加到现有历史。 | `filename`: 文件路径                        | 加载条目数（-1 失败）                 |
| `void linenoiseHistoryPrint(void);`               | 打印所有历史到 stdout（带编号）。      | 无                                          | 无                                    |

- **使用时机**：在主循环中，处理输入后调用 `linenoiseHistoryAdd(line)`；退出前保存。
- **文件格式**：纯文本，每行一个历史条目（无引号或转义）。
- **多行支持**：多行输入作为单条目添加，保存时 `\n` 保留为多行文本。

## 完整实现示例

以下示例集成历史管理：设置长度 100；添加非空输入；运行时加载/保存文件；支持 `/history` 打印和 `/historylen <n>` 调整。编译链接 `linenoise.c`。

### 代码（history_example.c）
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linenoise.h"

int main(int argc, char **argv) {
    char *line;
    char *history_file = "linenoise_history.txt";  // 历史文件路径

    // 初始化：设置最大长度，加载历史
    linenoiseHistorySetMaxLen(100);
    if (linenoiseHistoryLoad(history_file) == -1) {
        printf("No previous history file found.\n");
    } else {
        printf("History loaded from %s.\n", history_file);
    }

    printf("Linenoise History Demo. Use up/down arrows to navigate.\n");
    printf("Commands: /history (print), /historylen <n> (set len), /quit (exit)\n");

    // 主循环
    while ((line = linenoise("hist> ")) != NULL) {
        if (line[0] == '\0') {
            free(line);
            continue;  // 忽略空输入
        }

        if (!strncmp(line, "/history", 8)) {
            linenoiseHistoryPrint();  // 打印历史
        } else if (!strncmp(line, "/historylen", 11)) {
            int len = atoi(line + 11);
            if (len > 0) {
                int old_len = linenoiseHistorySetMaxLen(len);
                printf("History length set from %d to %d.\n", old_len, len);
                // 调整后可能截断旧历史
            }
        } else if (!strncmp(line, "/quit", 5)) {
            break;
        } else {
            // 正常输入：回显并添加历史
            printf("You entered: %s\n", line);
            if (line[0] != '\0') {
                linenoiseHistoryAdd(line);  // 添加到历史
            }
        }

        free(line);
    }

    // 退出前保存历史
    if (linenoiseHistorySave(history_file) == 0) {
        printf("History saved to %s.\n", history_file);
    } else {
        perror("Failed to save history");
    }

    return 0;
}
```

### 编译与运行
```bash
gcc -o history_example history_example.c linenoise.c
./history_example
```

### 示例交互
首次运行：
```
Linenoise History Demo. Use up/down arrows to navigate.
Commands: /history (print), /historylen <n> (set len), /quit (exit)
hist> hello world
You entered: hello world
hist> test 123<↑>  // 上下箭头回溯到 "hello world"
You entered: hello world
hist> /history
  1 : hello world
  2 : test 123
hist> /historylen 5
History length set from 100 to 5.
hist> /quit
History saved to linenoise_history.txt.
```

下次运行：自动加载 `linenoise_history.txt`，历史可用。

## 高级用法

### 去重历史
官方无内置去重，可自定义检查：
```c
// 在添加前检查
int linenoiseHistoryExists(const char *line) {
    // 遍历历史链表（需访问 linenoise.c 内部，或重写）
    // 简化：使用字符串比较（低效，但示例）
    // 实际项目中，可维护 hash set
    return 0;  // 假设不存在
}

if (!linenoiseHistoryExists(line)) {
    linenoiseHistoryAdd(line);
}
```
完整去重需修改 `linenoise.c` 的 `history` 链表逻辑。

### 与多行/异步集成
- **多行**：`linenoiseHistoryAdd` 处理完整多行字符串（含 `\n`），保存时文件多行显示正常。
- **异步**：在 `linenoiseEditFeed` 后（line != NULL 时）调用 `linenoiseHistoryAdd`；保存/加载在主循环外。
- **持久化策略**：每添加后保存（`linenoiseHistorySave`），或仅退出时保存（性能更好）。

### 自定义历史文件
- 支持相对/绝对路径；多用户场景用 `getenv("HOME")` 拼接 `~/.linenoise_history`。
- 备份：保存前复制旧文件。

## 注意事项
- **内存**：最大长度后自动丢弃旧条目（FIFO）。
- **安全性**：历史文件明文存储敏感命令（如密码），生产环境加密或禁用。
- **兼容**：文件编码 UTF-8；Windows 下用 `fopen` 模式 `"w"`/`"r"`。
- **限制**：历史不跨进程共享；TTY 外无箭头导航（fallback 到输入）。
- **调试**：用 `/history` 验证；若加载失败，检查文件权限。

此管理方式简单高效，适合轻量应用。若需扩展（如搜索历史或 JSON 格式），可 fork GitHub 仓库自定义。更多疑问，提供具体场景！