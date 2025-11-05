## Linenoise-NG 的使用

### GNU Readline 的替代品

GNU Readline 是一个广泛用于命令行交互的库，提供输入编辑、历史记录和补全等功能。但由于其 GPL 许可限制和复杂性，许多开发者寻求更轻量、许可更宽松的替代品。以下是常见的替代方案，主要基于 C/C++ 实现（适用于命令行工具开发），我根据流行度和功能整理了几个主要选项。这些替代品通常支持类似的核心功能，如键绑定、历史记录和多行编辑。这些替代品通常更容易集成，且避免了 GPL 的链接限制。

| 名称 | 描述 | 关键特性 |
| ------------ | ------------------------------------ | ------------------------------------ | 
| **libedit**      | BSD 许可的 Readline 兼容库，常作为系统默认替代（如 FreeBSD、macOS）。 | 基本编辑、历史记录、Emacs/vi 模式；与 Readline API 高度兼容。 | 
| **Linenoise**    | 轻量级、自包含的 Readline 替代，已被 Redis、MongoDB 和 Android 采用。 | 单/多行编辑、常见键绑定、历史记录；零配置，小巧（~10KB）。    | 
| **Linenoise-NG** | Linenoise 的增强版，支持 UTF-8 和跨平台移植。| UTF-8 处理、历史记录、补全；便携性强，无外部依赖。| 
| **replxx**       | 现代 C/C++ 库，支持语法高亮和 Unicode。 | 语法高亮、提示、Tab 补全、历史记录；C 和 C++ 接口。 | 
| **Bestline**     | 极简自包含库，专注于性能和许可灵活性。  | 基本编辑、历史记录；无依赖，轻量级。 | 

: GNU Readline 的替代品{.striped}

选择建议：

- 如果你追求兼容性和稳定性，优先 libedit（它直接兼容 GNU Readline，许多系统已内置）。
- 对于轻量嵌入式 项目，Linenoise 或 Linenoise-NG 是首选，已在生产环境中验证。
- 注意 replxx 生成的 Windows CLI 程序在输入时出现“抖动”现象。

### 使用 MSVC 2019 编译 Linenoise-NG

Linenoise-NG 支持使用 Visual Studio 2019（MSVC）通过 CMake 构建，这是 Windows 平台的推荐方式。以下基于官方 GitHub 仓库的最新说明，提供详细步骤。确保你已安装 Visual Studio 2019（包含 C++ 桌面开发工作负载）和 CMake（版本 3.15 或更高）。

::: important

GitHub 仓库 <https://github.com/arangodb/linenoise-ng> 提供的 *CMakeLists.txt* 需做修改才能通过编译。 见 *[修复 MSVC 2019 链接错误]*

:::

**编译步骤**

1. 创建构建目录：
   ```bash
   md build
   cd build
   ```

2. 使用 CMake 生成 Visual Studio 解决方案文件：
   - **32 位（x86）**：
     ```bash
     cmake -G "Visual Studio 16 2019" -A Win32 -DCMAKE_BUILD_TYPE=Release ..
     ```
   - **64 位（x64）**：
     ```bash
     cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release ..
     ```

3. 打开生成的 `linenoise.sln` 文件：
   - 在 Visual Studio 2019 中打开该文件。
   - 选择“Release” 配置和相应平台（Win32 或 x64）。
   - 右键项目 > 生成（或按 Ctrl+Shift+B）。

编译完成后，会在 `build` 目录下生成库文件（如 `linenoise.lib`）。

**安装（可选）**

- 在 Visual Studio 中构建“INSTALL” 项目（如果有），或使用命令行：
  ```bash
  cmake --build . --config Release --target install
  ```
- 自定义安装路径：设置环境变量 `DESTDIR=/path/to/install`，然后运行安装命令。
- 默认安装到系统路径（如 `C:\Program Files`），可能需要管理员权限。

**注意事项**

- 如果 CMake 找不到 VS 2019，确保 Visual Studio 已正确安装并运行 `vcvarsall.bat` 设置环境（或使用“Developer Command Prompt for VS 2019”）。
- 原仓库旧文档提到 VS 2013（生成器 "Visual Studio 12 2013"），但 VS 2019 使用 "Visual Studio 16 2019" 生成器。
- 常见问题：如果构建失败，检查 C++11 支持或清理构建目录重试。
- 使用示例与之前相同，链接时指定生成的 `.lib` 文件（如 `cl example.c linenoise.lib /Fe:example.exe`）。

### 使用 nmake 命令编译 Linenoise-NG

Linenoise-NG 项目主要依赖 CMake 作为构建系统，通过 CMake 生成 NMake Makefiles，可以用命令行方式构建，而无需打开 Visual Studio。

**编译步骤**

1. 创建构建目录：
   ```bash
   md build
   cd build
   ```

2. 使用 CMake 生成 NMake Makefiles：
   ```bash
   cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
   ```
   - 这会生成 `Makefile` 文件，专为 nmake 设计。
   - 如果是 32 位：无需额外参数，默认 x86。
   - 常见选项：添加 `-A x64` 指定架构（CMake 3.21+ 支持）。

3. 使用 nmake 构建：
   ```
   nmake
   ```
   - 或指定目标：`nmake install`（安装到系统路径，需要管理员权限）。
   - 完整构建：`nmake /f Makefile VERBOSE=1`（显示详细输出，便于调试）。

库文件（如 `linenoise.lib`）会出现在 `build` 目录下。

### 修复 MSVC 2019 链接错误 

使用项目中的 *CMakeLists.txt* 编译时可能出现链接错误：`unresolved external symbol __imp____acrt_iob_func`。这个错误是典型的 MSVC 链接器问题（LNK2019），发生在使用 `printf`（或类似 C 运行时函数）时。原因分析：

- 链接命令中包含多个 `/NODEFAULTLIB` 标志（如 `/NODEFAULTLIB:msvcrt.lib`、`/NODEFAULTLIB:libcmtd.lib` 等），这些标志**排除所有默认 C 运行时库 (CRT)** 的自动链接。
- CRT 库（如 `msvcrt.lib` 用于 Release 多线程 DLL 配置）包含 `__acrt_iob_func`（用于 I/O 缓冲区的内部函数，由 VS2015+ 引入，替换已弃用的 `__iob_func`）。
- 由于所有 CRT 被排除，但代码（如 `example.c` 中的 `printf`）依赖 CRT，链接器找不到符号定义，导致失败。
- 这可能是 linenoise-ng 的 CMake 配置中为避免 CRT 版本混用而设置的链接标志（常见于跨配置构建），但在 Windows Release 配置下过度排除，导致无 CRT 可用。库 `linenoise.lib` 本身构建成功，但示例应用缺少运行时支持。

退出码 1120 表示链接阶段失败。以下是修复步骤：


打开 **Developer Command Prompt for VS 2019**（x86 或 x64，根据你的构建匹配；搜索 "Developer Command Prompt"）。假设源代码在 `C:\Users\USERNAME\Downloads\linenoise-ng-master`，构建在 `build`（Release 配置），`example.c` 在 `tst\example.c`。

编辑 `CMakeLists.txt`（或 `tst/CMakeLists.txt`，如果存在）移除/调整排除标志，或显式链接 CRT。

**步骤：**

1. 打开 `linenoise-ng-master/CMakeLists.txt`（用 VS Code 或记事本）。
2. 查找设置链接标志的行，如：
   - `set(CMAKE_EXE_LINKER_FLAGS ... /NODEFAULTLIB:...)`
   - `target_link_options(linenoise ... /NODEFAULTLIB:...)`
   - 或 `if(WIN32)` 块中类似。
   
   **修改：**
   - 对于 Release 配置，只排除调试库（libcmtd.lib、msvcrtd.lib、libcd.lib），保留 `msvcrt.lib`。
   - 示例修复（添加/修改）：
     ```cmake
     if(MSVC)
       if(CMAKE_BUILD_TYPE STREQUAL "Release")
         set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libc.lib")
         # 不要排除 msvcrt.lib
       endif()
     endif()
     ```
   - 或者，在 `add_executable(example ...)` 后添加：
     ```cmake
     target_link_libraries(example PRIVATE msvcrt ucrt vcruntime)
     ```
     - `msvcrt.lib`：Release 多线程 DLL CRT。
     - `ucrt.lib`：Universal CRT（VS2015+ 必需）。
     - `vcruntime.lib`：VC 运行时。

3. 清理并重建：
   ```
   cd build
   del /Q *
   cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
   nmake
   ```
   - 或用 VS：重新生成解决方案并构建。

4. 如果是 nmake，检查生成的 `Makefile` 中的链接命令，确认无 `/NODEFAULTLIB:msvcrt.lib`。

### API

Linenoise 是 Salvatore Sanfilippo (antirez) 开发的轻量级命令行编辑库，旨在作为 GNU Readline 的简单替代品，主要支持 Unix-like 系统，代码纯 C 实现。Linenoise-ng 是其 fork 版本（由 ArangoDB 等维护），在保持原 API 兼容性的前提下，添加了 UTF-8 支持、Windows 兼容性和一些增强功能，使用 C++ 内部实现，但对外提供纯 C 接口。总体上，两个库的 API 高度兼容，用户代码几乎无需修改即可从 Linenoise 迁移到 Linenoise-ng。主要差异在于 Linenoise-ng 添加了少量新函数和版本宏，以支持更多场景。

共同 API
两个库的核心 API 完全相同，包括基本输入、历史记录、补全、提示等功能。以下是主要函数列表（基于 Linenoise 的 README 文档，Linenoise-ng 兼容）：

**linenoise**

`char *linenoise(const char *prompt);`

显示提示符，支持行编辑和历史记录。返回用户输入的行（EOF 或 OOM 时返回 NULL）。需用 `free()` 或 `linenoiseFree()` 释放。

**linenoiseFree**

`void linenoiseFree(void *ptr);`

释放由 `linenoise` 分配的缓冲区，使用相同的分配器。

**linenoiseSetMultiLine**

`void linenoiseSetMultiLine(int ml);`

启用（ml != 0）或禁用多行编辑。

**linenoiseHistoryAdd**

`int linenoiseHistoryAdd(const char *line);`

将行添加到历史记录顶部。成功返回 0，错误返回 -1。

**linenoiseHistorySetMaxLen**

`int linenoiseHistorySetMaxLen(int len);`

设置历史记录最大条目数（0 禁用历史）。成功返回 0，错误返回 -1。

**linenoiseHistorySave**

`int linenoiseHistorySave(const char *filename);`

将历史记录持久化到文件。成功返回 0，错误返回 -1。

**linenoiseHistoryLoad**

`int linenoiseHistoryLoad(const char *filename);`

从文件加载历史记录。成功返回 0，错误返回 -1。

**linenoiseMaskModeEnable**

`void linenoiseMaskModeEnable(void);`

启用掩码模式（输入字符显示为 `*`）。

**linenoiseMaskModeDisable**

`void linenoiseMaskModeDisable(void);`

禁用掩码模式。

**linenoiseSetCompletionCallback**

`void linenoiseSetCompletionCallback(void (*fn)(const char *, linenoiseCompletions *));`

注册补全回调函数。回调接收当前缓冲区和补全列表，调用 `linenoiseAddCompletion` 添加选项。

**linenoiseAddCompletion**

`void linenoiseAddCompletion(linenoiseCompletions *lc, const char *str);`

向补全列表添加单个字符串。

**linenoiseSetHintsCallback**

`void linenoiseSetHintsCallback(char *(*fn)(const char *, int *, int *));`

注册提示回调函数。返回提示字符串（NULL 无提示），并设置颜色和粗体参数。

**linenoiseSetFreeHintsCallback**

`void linenoiseSetFreeHintsCallback(void (*fn)(void *));`

注册释放动态分配提示字符串的函数。

**linenoiseClearScreen**

`void linenoiseClearScreen(void);`

清屏。

**linenoiseEditStart**

`void linenoiseEditStart(struct linenoiseState *ls, int fd_in, int fd_out, char *buf, size_t buflen, const char *prompt);`

初始化异步（多路复用）编辑上下文。

**linenoiseEditFeed**

`char *linenoiseEditFeed(struct linenoiseState *ls);`

馈入输入文件描述符的下一个字符。返回 `linenoiseEditMore` 表示继续编辑，行完成时返回分配的行，EOF/错误返回 NULL。

**linenoiseEditStop**

`void linenoiseEditStop(struct linenoiseState *ls);`

清理编辑上下文。

**linenoiseHide**

`void linenoiseHide(struct linenoiseState *ls);`

隐藏当前编辑行（允许打印其他输出）。

**linenoiseShow**

`void linenoiseShow(struct linenoiseState *ls);`

恢复隐藏的编辑行。

**类型定义**（共同）：
  
- `struct linenoiseCompletions { size_t len; char **cvec; };`、
- `typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);` 
- 等。

Linenoise-ng 的新增 API

Linenoise-ng 在原基础上添加了以下功能，主要用于增强可用性和配置：

版本宏（用于检查兼容性）：

#define LINENOISE_VERSION "1.0.0"（版本字符串）
#define LINENOISE_VERSION_MAJOR 1
#define LINENOISE_VERSION_MINOR 0
其他变体如 LINENOISE_VERSION_PATCH、LINENOISE_VERSION_INT 等（Linenoise 无这些）。


void linenoisePreloadBuffer(const char *preloadText);：预加载默认文本到输入缓冲区，便于编辑默认值（如命令补全后的默认参数）。这是原 Linenoise 缺少的功能，用户常需手动实现。
