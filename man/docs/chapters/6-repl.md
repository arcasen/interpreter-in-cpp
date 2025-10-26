## REPL 的实现

### 简单 REPL 实现（C 语言）

以下是一个用 C 语言实现的简单 REPL（Read-Eval-Print Loop）。它的功能是：
- 持续读取用户输入的一行文本。
- 立即回显（echo）该行。
- 支持通过输入 "exit" 来退出循环（可选退出条件，以使 REPL 更实用）。
- 使用 `fgets` 读取输入行，支持处理空格等字符。

#### 完整代码
```c
#include <stdio.h>
#include <string.h>

#define MAX_LINE 1024  // 最大输入行长度

int main() {
    char line[MAX_LINE];
    
    printf("欢迎进入简单 REPL！输入 'exit' 退出。\n");
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        // 回显用户输入的行（包括换行符）
        printf("Echo: %s", line);
        
        // 检查是否为退出命令（去除换行符后比较）
        line[strcspn(line, "\n")] = '\0';  // 移除换行符
        if (strcmp(line, "exit") == 0) {
            printf("退出 REPL。\n");
            break;
        }
    }
    
    return 0;
}
```

#### 编译和运行
1. 保存代码为 `repl.c`。
2. 编译：`gcc repl.c -o repl`（需要安装 GCC 编译器）。
3. 运行：`./repl`。
4. 示例交互：
   ```
   欢迎进入简单 REPL！输入 'exit' 退出。
   Hello World!
   Echo: Hello World!
   
   这是一行测试
   Echo: 这是一行测试
   
   exit
   Echo: exit
   退出 REPL。
   ```

#### 说明
- **读取输入**：`fgets` 函数安全地读取一行（最多 `MAX_LINE` 字符），直到遇到换行符或 EOF。
- **回显**：直接打印 `line`，保留原样（包括尾部的换行符）。
- **退出机制**：使用 `strcmp` 检查是否为 "exit"（忽略大小写可进一步优化，但这里保持简单）。如果不需要退出，可以移除 `if` 块，使其无限循环直到 Ctrl+D (EOF)。
- **潜在改进**：如果需要处理更长的输入，增大 `MAX_LINE`；或使用 `readline` 库支持历史记录（需链接 `-lreadline`）。

### 改进 REPL：支持退格和历史记录

基于之前的简单 REPL，添加了以下功能：
- **退格支持**：允许在输入过程中使用退格键（Backspace）删除字符，支持光标移动、插入等基本行编辑。
- **历史记录**：保存之前的输入命令，支持向上/向下箭头键（↑/↓）导航历史记录。历史文件默认为 `~/.repl_history`，最多保存 100 条记录。

这些功能依赖 **GNU Readline** 库（一个标准的 C 行编辑库），它提供了完整的终端交互支持。如果你的系统没有安装，可以通过包管理器安装：
- Ubuntu/Debian: `sudo apt install libreadline-dev`
- macOS: `brew install readline`
- CentOS/RHEL: `sudo yum install readline-devel`

#### 完整代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#define HISTFILE ".repl_history"  // 历史文件路径
#define MAX_HIST 100              // 最大历史记录数

int main() {
    char *line;
    int histfile = 0;  // 历史文件扩展名

    // 读取历史文件
    read_history(HISTFILE);
    stifle_history(MAX_HIST);  // 限制历史大小

    printf("欢迎进入增强 REPL！支持退格、箭头键历史导航。输入 'exit' 退出。\n");

    while ((line = readline("> ")) != NULL) {
        // 添加空行跳过
        if (*line == '\0') {
            free(line);
            continue;
        }

        // 回显用户输入的行
        printf("Echo: %s\n", line);

        // 添加到历史记录
        if (strlen(line) > 0) {
            add_history(line);
        }

        // 检查退出命令
        if (strcmp(line, "exit") == 0) {
            printf("退出 REPL。\n");
            break;
        }

        free(line);
    }

    // 保存历史到文件
    write_history(HISTFILE);

    return 0;
}
```

#### 编译和运行
1. 保存代码为 `repl_enhanced.c`。
2. 编译：`gcc repl_enhanced.c -o repl_enhanced -lreadline`（注意链接 `-lreadline`）。
3. 运行：`./repl_enhanced`。
4. 示例交互（在终端中）：
   ```
   欢迎进入增强 REPL！支持退格、箭头键历史导航。输入 'exit' 退出。
   > Hello World!
   Echo: Hello World!
   > 这是一行测试 [按退格删除“试”字，然后回车]
   Echo: 这是一行测
   > [按 ↑ 键]  # 会显示上一个命令 "这是一行测"，可编辑后回车
   Echo: 这是一行测 (编辑版)
   > exit
   Echo: exit
   退出 REPL。
   ```
   - 历史记录会持久化：下次运行时，↑/↓ 键可导航之前的命令。
   - 支持 Tab 补全（如果 Readline 配置了）、Ctrl+C 中断输入等。

#### 说明
- **Readline 集成**：
  - `readline("> ")`：读取一行，支持退格、箭头键、历史导航。
  - `add_history(line)`：将非空输入添加到历史。
  - `read_history` / `write_history`：加载/保存历史文件到用户主目录。
- **限制**：历史最多 100 条（可调整 `MAX_HIST`）。如果输入为空，直接跳过。
- **无 Readline 依赖的替代**：如果不想用外部库，可以手动实现行编辑（使用 `termios` 处理键码），但代码会复杂得多（约 200+ 行）。

这个版本更接近真实 REPL（如 Python 的 Shell）。


### 在 Windows 上编译

Readline 是 GNU 项目的一个 C 库，主要针对 Unix/Linux 系统设计，提供命令行编辑（如退格、历史导航）。Windows 的命令行（CMD/PowerShell）有自己的输入机制，但不支持 Readline 的高级功能（如箭头键历史、Tab 补全）。Microsoft 的控制台 API（如 ReadConsole）可以实现基本输入，但不等同于 Readline。集成 Readline 到 C 项目中（基于之前的 REPL 代码）。推荐使用 MinGW 或 Dev-C++ 等工具链，这些支持 GCC 和 GNU 库。

使用 MinGW-w64（更灵活，适合 VS Code/Clion）：

安装 MinGW-w64 或通过 MSYS2。在 MSYS2 终端运行：

```bash
pacman -S mingw-w64-x86_64-readline mingw-w64-x86_64-ncurses #（ncurses 是依赖）。
```

编译命令：

```
gcc repl_enhanced.c -o repl_enhanced -lreadline -lhistory
```

如果用 VS Code，安装 C/C++ 扩展并配置 MinGW 路径。
