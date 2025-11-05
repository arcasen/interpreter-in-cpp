# Linenoise 使用教程

Linenoise 是一个轻量级的命令行编辑库（line editing library），由 Redis 作者 Salvatore Sanfilippo（antirez）开发。它是 GNU Readline 的简化替代品，仅约 850 行代码，支持单/多行编辑、历史记录、自动补全、提示等功能。广泛用于 Redis、MongoDB 和 Android 等项目。BSD 许可，零配置，易于集成。

本教程基于官方文档和中文资源，涵盖从安装到高级用法的完整指南。适用于 C 语言开发，适合实现 REPL（Read-Eval-Print Loop）交互式命令行。

## 1. 下载与安装

Linenoise 无需复杂安装，直接从 GitHub 下载源码集成到项目中。

### 步骤：
1. 访问 [GitHub 仓库](https://github.com/antirez/linenoise)，点击 **Code > Download ZIP** 下载最新版本（或使用 tag 下载特定版本）。
2. 解压到项目目录，例如命名为 `linenoise`。
3. 在项目中包含 `linenoise.h` 和 `linenoise.c` 文件。

### 使用 CMake 构建示例项目
创建项目目录 `linenoise_test`，结构如下：
```
linenoise_test/
├── linenoise/          # 解压的 linenoise 源码
├── src/
│   └── main.c          # 你的主程序
├── build/              # 构建目录
└── CMakeLists.txt
```

**main.c**（基本示例）：
```c
#include <stdio.h>
#include <stdlib.h>
#include "linenoise.h"

int main() {
    char *line;
    while ((line = linenoise("hello> ")) != NULL) {
        printf("You wrote: %s\n", line);
        free(line);  // 或使用 linenoiseFree(line)
    }
    return 0;
}
```

**CMakeLists.txt**：
```cmake
cmake_minimum_required(VERSION 3.5)
project(demo)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/linenoise)

set(SRC_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/linenoise/linenoise.c
)

add_executable(demo ${SRC_FILE})
```

### 编译与运行
```bash
cd build
cmake .. && make
./demo
```
输出示例：
```
hello> test input
You wrote: test input
hello> 
```
用户可使用箭头键导航、退格删除等。

## 2. 基本用法

核心 API：`char *linenoise(const char *prompt);`
- `prompt`：提示符字符串（如 `"hello> "`）。
- 返回：用户输入的字符串（需 `free()` 释放），EOF 或 OOM 时返回 `NULL`。
- 非 TTY 模式下无长度限制（TTY 下限 `LINENOISE_MAX_LINE`）。

典型循环：
```c
char *line;
while ((line = linenoise("hello> ")) != NULL) {
    // 处理输入
    printf("Echo: '%s'\n", line);
    free(line);
}
```

### 单行 vs 多行编辑
默认单行编辑（适合短输入）。启用多行：
```c
linenoiseSetMultiLine(1);  // 启用
linenoiseSetMultiLine(0);  // 禁用
```
多行模式使用 Enter 换行，Ctrl+C 取消。

### 彩色提示符
使用 ANSI 转义码：
```c
#define GREEN "\033[0;32m"
#define NONE "\033[m"
linenoise(GREEN "hello> " NONE);
```
效果：绿色提示符。

## 3. 高级功能

### 历史记录
支持上下箭头导航历史。默认历史长度 0（禁用）。

API：
- `int linenoiseHistoryAdd(const char *line);`：添加命令到历史。
- `int linenoiseHistorySetMaxLen(int len);`：设置最大长度（如 100）。
- `int linenoiseHistorySave(const char *filename);`：保存到文件（返回 0 成功）。
- `int linenoiseHistoryLoad(const char *filename);`：加载历史。

示例：
```c
linenoiseHistorySetMaxLen(100);
while ((line = linenoise("hello> ")) != NULL) {
    if (line[0] != '\0' && line[0] != '/') {
        printf("Echo: '%s'\n", line);
        linenoiseHistoryAdd(line);
        linenoiseHistorySave("history.txt");
    } else if (!strncmp(line, "/historylen", 11)) {
        int len = atoi(line + 11);
        linenoiseHistorySetMaxLen(len);
    }
    free(line);
}
```
输入 `/historylen 50` 调整长度。

### 自动补全（Tab 键）
注册回调函数，按 Tab 触发补全。

API：
- `void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);`：注册回调。
- `void linenoiseAddCompletion(linenoiseCompletions *lc, const char *str);`：添加补全项。

示例回调：
```c
void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == 'h') {
        linenoiseAddCompletion(lc, "hello");
        linenoiseAddCompletion(lc, "hello there");
    }
}

int main() {
    linenoiseSetCompletionCallback(completion);
    // ... 循环
}
```
输入 `h` + Tab：补全为 `hello`，再 Tab 切换 `hello there`。

### 提示（Hints）
在光标右侧显示建议，支持颜色/粗体。

API：
- `void linenoiseSetHintsCallback(linenoiseHintsCallback *);`：注册回调。
- 可选：`void linenoiseSetFreeHintsCallback(linenoiseFreeHintsCallback *);`：释放动态分配的提示。

示例：
```c
char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf, "git remote add")) {
        *color = 35;  // 品红
        *bold = 0;
        return " <name> <url>";
    }
    return NULL;
}

int main() {
    linenoiseSetHintsCallback(hints);
    // ... 循环
}
```
颜色码：红=31, 绿=32, 黄=33, 蓝=34, 品红=35, 青=36, 白=37。

### 掩码模式（密码输入）
隐藏输入为 `*`：
```c
void linenoiseMaskModeEnable(void);   // 启用
void linenoiseMaskModeDisable(void);  // 禁用
```
示例：输入密码时启用，完成后禁用。

### 清屏
```c
void linenoiseClearScreen(void);
```
在命令中调用，如输入 `clear` 时执行。

## 4. 异步模式（高级）
适用于需同时处理网络/事件输入的场景（如 IRC 客户端）。使用 `struct linenoiseState` 管理状态。

核心 API：
- `void linenoiseEditStart(struct linenoiseState *ls, int stdin_fd, int stdout_fd, char *buf, size_t buflen, const char *prompt);`：启动编辑。
- `char *linenoiseEditFeed(struct linenoiseState *ls);`：馈入输入，返回 `linenoiseEditMore` 表示继续。
- `void linenoiseEditStop(struct linenoiseState *ls);`：停止编辑。
- `void linenoiseHide(struct linenoiseState *ls);` / `void linenoiseShow(struct linenoiseState *ls);`：隐藏/显示当前行（用于异步输出）。

示例需结合 `select(2)` 处理事件，详见官方 `example.c` 中的 `--async` 模式。

## 5. 注意事项
- **依赖**：仅需标准 C 库，支持 VT100 转义（ANSI 兼容）。
- **移植**：嵌入式系统可替换 `write` 为 `printf`，添加 `memset` 初始化。
- **C++ 版本**：若需 C++，可使用 cpp-linenoise 包装。
- **限制**：TTY 模式下行长有限制；无 GUI 支持。

## 6. 完整示例项目
参考官方 `example.c`：集成历史、补全、提示。编译时添加 `linenoise/example.c` 到 CMake。

更多资源：
- 官方 README：[GitHub](https://github.com/antirez/linenoise)
- 对比 Readline：[博客](http://liuyang1.github.io/2015/02/01/linenoise/)

通过本教程，你可以快速在项目中集成交互式命令行。若需特定功能扩展，欢迎提供更多细节！