## Panic Mode 错误恢复 （ Panic Mode Error Recovery ）

在编译原理（Compiler Design）中，Panic Mode（恐慌模式）是一种常见的语法错误恢复策略，主要用于语法分析（Parsing）阶段。 Panic Mode 是处理编译时语法错误的最简单且最流行的方法之一，当解析器遇到无法处理的无效输入时，通过“恐慌”方式快速恢复，继续分析剩余代码，从而避免整个编译过程崩溃。基本思想是一旦检测到错误，语法分析器就“恐慌”起来，认为当前所处的状态（比如正在分析一个结构）已经不可靠了。于是丢弃后续的输入符号（Token），直到找到一个同步点（synchronization points），然后从该点重新开始分析。

现在要分析典型的四则运算表达式文法（递归下降风格）的同步点（synchronization points）。
通常，同步点会选择在一个非终结符的 FIRST 集或一个规则结束后的 FOLLOW 集中的 Token。  

在编译器设计中的 panic mode 错误恢复策略下，如果当前 token 位于错误位置（即不匹配当前非终结符的预测集，导致语法错误），且它同时是一个同步点（synchronization points，例如 `;`、`}` 等属于同步集的符号），则不需要跳过它。

**简要解释**

- Panic Mode 的错误恢复流程：检测到错误后，解析器进入“恐慌”模式，反复跳过（丢弃）当前 token，直到当前 token 属于同步集（sync set）。同步集是预定义的符号集合，用于“重置”解析上下文（如语句结束符、块分隔符），以最小化错误传播并继续解析后续有效代码。
- 当前 token 是同步点的情况：即使这个 token 导致了错误（即它是“错误位置”），但因为它已经是同步点，跳过循环会立即停止。解析器会保留并消费这个同步 token，将其作为锚点来恢复正常解析。例如：
  - 如果预期一个表达式，但遇到了 `;`（同步点），即使 `;` 在此位置是错误的，也不会跳过它，而是用它结束当前语句并转向下一个语句的解析。
- 为什么不跳过？
  - 跳过同步点会破坏重新同步的机会，导致解析器继续“恐慌”，可能丢弃更多有效代码。
  - 目标是“宽容”错误：用同步点快速“跳出”当前错误上下文，而不是过度丢弃输入。
  - 这是一种权衡：可能遗漏部分代码，但能让编译器报告错误并继续处理文件其余部分。

伪代码示例（基于 LL(1) 或类似预测解析）：

```c
if (当前 token 不匹配预期 FIRST/FOLLOW 集) {  // 检测到错误
    报告错误("Unexpected token: " + 当前 token);
    
    while (当前 token 不在同步集) {  // 跳过循环
        消费当前 token;  // 丢弃
        读取下一个 token;
    }
    // 循环结束：当前 token 是同步点（即使它是错误位置）
    // 直接消费它作为同步锚点，继续解析下一个产生式
    匹配当前 token;  // 保留并使用它
}
```
- 如果进入 while 时，当前 token 就是同步点，循环体不执行，因此不跳过。

### 非终结符的 FIRST 集合 和 FOLLOW 集合

四则运算表达式文法的非终结符和终结符：

- 非终结符：`expr`, `term`, `factor`。  
- 终结符：`+`, `-`, `*`, `/`, `(`, `)`, `digit`，以及输入结束 `$`。（ `integer` 可视为终结符 `digit` 或直接处理为数字令牌）

FIRST 集合：

- FIRST(factor) = { `(`, `digit` }
- FIRST(term) = { `(`, `digit` }
- FIRST(expr) = { `(`, `digit` }

FOLLOW 集合：

- FOLLOW(expr)   = { `)`, `$` }
- FOLLOW(term)   = { `+`, `-`, `)`, `$` }
- FOLLOW(factor) = { `*`, `/`, `+`, `-`, `)`, `$` }

### 同步点的选择

在递归下降分析中：

- 进入某个非终结符的函数时，如果当前 Token 不在它的 FIRST 集里，可以报错，并跳过 Token 直到遇到 FOLLOW 中的 Token。  
- FOLLOW 集里的 Token 可作为从该非终结符退出的同步点。  

在 Panic Mode 中，对于每个非终结符，使用其 FOLLOW 集中的终结符作为同步点（丢弃 Token 直到遇到这些）。这能确保恢复到“预期结束”位置，避免过度丢弃有效代码。

| 非终结符   | FOLLOW 集（同步点） | 解释                                         |
| --------- | -------------------- | ---------------------------------------- |
| **expr**   | {`)`, `$`}                     | 表达式结束于右括号或文件末尾。   |
| **term**   | {`+`, `-`, `)`, `$`}           | 项结束于加减运算符、右括号或末尾。 |
| **factor** | {`*`, `/`, `+`, `-`, `)`, `$`} | 因子结束于乘除/加减运算符、右括号或末尾。|

: 同步点的选择 {.striped}

同步点不一定只能选 FOLLOW 集。FOLLOW 集是推荐和最优的选择，但实际实现中可以灵活调整，甚至使用固定或自定义的同步点集。

### 同步点为什么不能（或不宜）用 FIRST 集

在自顶向下解析（如 LL(1) 解析器）的错误恢复机制中，同步点（synchronization points）的主要目的是在遇到错误时“同步”输入流，继续解析而非崩溃。通常采用 panic-mode 错误恢复：当栈顶非终结符 A 无法匹配当前输入时，跳过输入符号直到遇到可能跟随 A 后的终结符（即 FOLLOW(A) 中的符号），然后弹出 A 并继续。

#### 为什么用 FOLLOW 集作为同步点？

- 语义匹配：FOLLOW(A) 精确表示“在 A 后可能出现的终结符”，这符合同步的逻辑——它告诉解析器“在 A 结束后，输入应该是什么样的符号”，从而安全地恢复上下文。
  
- 避免无限循环：使用 FOLLOW 可以确保跳过后，继续从正确的位置解析更高层的结构（如在表达式中，遇到 + 后可以继续 term）。
  
- 标准实践：在经典编译器设计（如 Dragon Book）中，同步点明确定义为 FOLLOW 集。

#### 为什么不能（或不宜）用 FIRST 集？

- 语义不匹配：FIRST(A) 表示从 A 开始可能出现的首终结符，用于选择产生式（预测分析表中的行选择），而非“跟随后”的同步。它描述 A 的开头，不是结束后的预期符号。如果用 FIRST(A) 作为同步点，会导致：

- 错误跳过：例如，栈顶是 expr，但当前输入是 +（不匹配 FIRST(expr)），如果用 FIRST(expr) = { +, −, (, id, ... } 同步，会跳过 +（因为它在 FIRST 中），但 + 其实可能是 FOLLOW(expr) 中的符号，用于连接多个 term，导致解析中断。
  
- 过度匹配：FIRST 集往往较大（包含多种可能起点），容易跳过多余输入，丢失上下文或造成新错误。
不安全：在嵌套结构中（如括号内 expr），用 FIRST 可能误判内部/外部边界，导致栈污染或无限递归。


### 使用 FIRST 和 FOLLOW 集检查错误

在递归下降分析中，FIRST 集 用于预测和选择产生式（决定是否进入某个非终结符的解析），而 FOLLOW 集 用于错误恢复和结束验证（同步输入，确保非终结符后跟的符号正确）。同时使用它们能实现更鲁棒的错误检测：FIRST 驱动解析过程，FOLLOW 提供“安全点”来恢复，避免级联错误。

同时使用的核心原理：

- **FIRST 的作用**：在函数入口，检查 lookahead $\in$ FIRST(A) 以选择产生式或进入空产生式。如果不匹配，立即报错并用 FOLLOW 恢复。

- **FOLLOW 的作用**：

  - 对于可空非终结符，函数结束时验证 lookahead $\in$ FOLLOW(A)，检测多余符号。
  - 错误恢复时，跳过输入直到 lookahead $\in$ FOLLOW(A)，然后重试解析。


- **结合优势**：FIRST 确保唯一路径（LL(1)），FOLLOW 提供上下文同步（如在表达式中，FOLLOW(E) 包含 ) 或 $，允许在括号后结束）。

如果文法有多个产生式，使用预测分析表（基于 FIRST/FOLLOW）加速选择。

用 Python 实现了递归下降解析器。该实现针对简单算术表达式文法，并同时使用 FIRST 和 FOLLOW 集进行错误检查和恢复。文法如下：

```ebnf
E ::= T {+ T}
T ::= F {* F}
F :: id | ( E )
```

Python 代码：

```python{.numberLines}
class Parser:
    def __init__(self, tokens):
        self.tokens = tokens + ['$']  # Add end marker
        self.pos = 0
        self.lookahead = self.tokens[self.pos]
        self.pos += 1
        self.current_non_terminal = None
        
        # Precomputed FIRST and FOLLOW sets for the grammar:
        # E -> T {+ T}
        # T -> F {* F}
        # F -> id | ( E )
        # Assuming D is replaced by id | ( E ) for a standard expression grammar
        self.FIRST = {
            'E': {'id', '('},
            'T': {'id', '('},
            'F': {'id', '('}
        }
        self.FOLLOW = {
            'E': {')', '$'},
            'T': {'+', ')', '$'},
            'F': {'*', '+', ')', '$'}
        }
    
    def advance(self):
        if self.pos < len(self.tokens):
            self.lookahead = self.tokens[self.pos]
            self.pos += 1
    
    def match(self, expected, context=""):
        if self.lookahead == expected:
            self.advance()
            return True
        else:
            self.report_error(f"Expected {expected} but got {self.lookahead} ({context})")
            return False
    
    def report_error(self, msg):
        print(f"Syntax error: {msg} at position {self.pos - 1}, token: {self.lookahead}")
        return self.sync()
    
    def sync(self):
        while self.lookahead not in self.FOLLOW[self.current_non_terminal]:
            if self.lookahead == '$':
                print("Unrecoverable error: end of input")
                return False
            self.advance()
        print(f"Recovery: skipped to FOLLOW symbol {self.lookahead}")
        return True
    
    def parse_E(self):
        self.current_non_terminal = 'E'
        if self.lookahead not in self.FIRST['E']:
            if not self.report_error(f"Unexpected token: {self.lookahead}, expected FIRST(E): {self.FIRST['E']}"):
                return False
            return False  # E cannot be empty
        
        self.parse_T()
        # Handle {+ T} loop
        while self.lookahead == '+':
            self.match('+', "additive operator")
            self.parse_T()
        
        if self.lookahead not in self.FOLLOW['E'] and self.lookahead != '$':
            self.report_error(f"Extra token after E: {self.lookahead}, expected FOLLOW(E): {self.FOLLOW['E']}")
        
        return True
    
    def parse_T(self):
        self.current_non_terminal = 'T'
        if self.lookahead not in self.FIRST['T']:
            self.report_error(f"Unexpected token: {self.lookahead}, expected FIRST(T): {self.FIRST['T']}")
            return False
        
        self.parse_F()
        # Handle {* F} loop
        while self.lookahead == '*':
            self.match('*', "multiplicative operator")
            self.parse_F()
        
        # 新增：FOLLOW 检查（结束验证）
        if self.lookahead not in self.FOLLOW['T'] and self.lookahead != '$':
            self.report_error(f"Extra token after T: {self.lookahead}, expected FOLLOW(T): {self.FOLLOW['T']}")
        
        return True
    
    def parse_F(self):
        self.current_non_terminal = 'F'
        if self.lookahead not in self.FIRST['F']:
            self.report_error(f"Unexpected token: {self.lookahead}, expected FIRST(F): {self.FIRST['F']}")
            return False
        
        if self.lookahead == 'id':
            self.match('id', "factor identifier")
        elif self.lookahead == '(':
            self.match('(', "left parenthesis")
            self.parse_E()
            self.match(')', "right parenthesis")
        else:
            # Should not reach here due to FIRST check, but handle
            return False
        
        # 新增：FOLLOW 检查（结束验证）
        if self.lookahead not in self.FOLLOW['F'] and self.lookahead != '$':
            self.report_error(f"Extra token after F: {self.lookahead}, expected FOLLOW(F): {self.FOLLOW['F']}")
        
        return True
    
    def parse(self):
        success = self.parse_E()
        if success and self.lookahead == '$':
            print("Parse successful")
            return True
        else:
            if self.lookahead != '$':
                self.report_error(f"Extra input after end: {self.lookahead}")
            print("Parse failed")
            return False
```

### 避免同一位置多次报错的策略

递归下降解析器（Recursive Descent Parser）是一种自顶向下的LL(1)解析方法，每个非终结符对应一个函数。这种设计简单且易于实现，但错误处理是一个常见挑战：当输入token序列出现语法错误时，多个递归调用的函数可能会在**同一位置（即同一个token）**检测到错误，并反复报告相同的错误信息，导致输出冗余和混乱。
例如，在解析表达式时，如果遇到意外的token（如缺少操作数），expr、term、unary 等函数都可能在同一处报错：“Expected operand after '+'”。

**核心问题原因**

- 级联错误：错误传播到上层函数，导致多层函数都尝试报告。
- 无状态跟踪：默认实现中，每个函数独立检查当前token，而不共享错误状态。

**解决方案**

避免多次报错的关键是错误恢复（Error Recovery）和状态管理。

