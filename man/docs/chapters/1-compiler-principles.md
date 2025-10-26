## 编译原理基础

### 算术计算器文法

#### 简单计算器的文法

算术计算器的语法可以写成类似 The C Programming Language 附录中的文法：

```bnf
expr    ::= expr + term | expr - term
term    ::= term * factor | term / factor
factor  ::= ( expr ) | integer
integer ::= [0-9]-
```

#### 消除左递归后的文法

上述文法存在左递归，不适合自顶向下分析时产生无限递归，不适合手工编写解析器，需要使用 Yacc/Bison 工具来构造抽象语法树。消除左递归的文法如下：

```bnf
expr    ::= term expr'
expr'   ::= + term expr' | - term expr' | ε
term    ::= factor term'
term'   ::= * factor term' | / factor term' | ε
factor  ::= ( expr ) | integer
integer ::= [0-9]+
```

#### EBNF 文法

消除左递归的文法略显复杂，以下是简化后 EBNF 的文法：

```ebnf
expr    ::= term { ( + | - ) term }
term    ::= factor { ( * | / ) factor }
factor  ::= ( expr ) | integer
integer ::= [0-9]+
```

这种 EBNF 形式更简洁，并且直接适用于许多解析器生成器。

### 递归下降分析（Recursive Descent Parsing）

递归下降分析是一种**自顶向下（Top-Down）**的语法分析（Parsing）方法，常用于编译器设计中的词法分析和语法分析阶段。它通过编写一组相互调用的函数（每个函数对应一个非终结符）来实现文法（Grammar）的解析。这种方法简单、直观，且易于实现，尤其适合**LL(1) 文法**（从左到右扫描、从左到右推导、预测一个符号）。

- **核心思想**：为文法中的每个**非终结符**（Non-terminal）编写一个递归函数。该函数尝试从当前输入符号（Token）开始，匹配该非终结符的产生式（Production）。
- **过程**：
  1. 从文法的**起始符号**（Start Symbol）开始调用解析函数。
  2. 函数内部根据当前输入符号的**预测分析表**（Predictive Parsing Table）或简单条件，选择合适的产生式。
  3. 如果产生式是终结符（Terminal），则直接匹配输入；如果是递归非终结符，则递归调用相应函数。
  4. 匹配成功后，继续处理下一个符号；失败则报错（可能回溯，但纯递归下降通常不需回溯）。
- **优点**：
  - 实现简单：直接用函数调用模拟文法规则，无需构建复杂的解析表。
  - 易于调试：每个函数独立，错误定位清晰。
  - 适合手写编译器或解释器。
- **缺点**：
  - 仅适用于**无左递归**（Left-Recursion）和**非歧义**的文法；否则需预处理文法。
  - 效率较低：递归调用可能导致栈溢出（深度过大）。
  - 不支持任意上下文无关文法（Context-Free Grammar）。
