## 增加数学函数与常量

### 修改后的文法

```
expr   ::= term { ( + | - ) term }
term   ::= unary { ( * | / ) unary }
unary  ::= ( + | - ) unary | power
power  ::= factor { ^ unary }  
factor ::= ( expr ) | id ( expr ) | id | integer | float
id     ::= [a-zA-Z]+[0-9]*
```

支持的常量有 `pi`、 `e` 和 `phi`，以及计算器上一次计算结果 `ans`。

$$
\pi = 3.1415926535897932384626434 
$$
$$
e = 2.7182818284590452353602875 
$$
$$
\phi = 1.6180339887498948482045868
$$

::: note

计算器不区分大小写，如：`Sin(pi/2)` 与 `sin(Pi/2)` 等价。

`M_PI`、`M_E` 非标准（POSIX 扩展），依赖它容易出问题，可以自己定义。

:::

### 支持的数学函数

计算器支持下面的函数用于进行各种数学运算，如三角函数、指数函数、对数函数、幂运算等。
按上述文法设计的计算器仅支持含一个参数的函数，多元函数的调用涉及处理参数列表的问题，需要修改文法。

| 函数名  | 原型                      | 描述               |
| ------- | ------------------------- | ------------------ |
| `sin`   | `double sin(double x);`             | 正弦函数（弧度）       |
| `cos`   | `double cos(double x);`             | 余弦函数（弧度）       |
| `tan`   | `double tan(double x);`             | 正切函数（弧度）       |
| `asin`  | `double asin(double x);`            | 反正弦函数（返回弧度） |
| `acos`  | `double acos(double x);`            | 反余弦函数（返回弧度） |
| `atan`  | `double atan(double x);`            | 反正切函数（返回弧度） |

: 三角函数（Trigonometric Functions）{.striped}


| 函数名  | 原型                      | 描述               |
| ------- | ------------------------- | ------------------ |
| `sinh`  | `double sinh(double x);`  | 双曲正弦           |
| `cosh`  | `double cosh(double x);`  | 双曲余弦           |
| `tanh`  | `double tanh(double x);`  | 双曲正切           |
| `asinh` | `double asinh(double x);` | 反双曲正弦（C99+） |
| `acosh` | `double acosh(double x);` | 反双曲余弦（C99+） |
| `atanh` | `double atanh(double x);` | 反双曲正切（C99+） |

: 双曲函数（Hyperbolic Functions）{.striped}


| 函数名  | 原型                      | 描述               |
| ------- | ------------------------- | ------------------ |
| `exp`   | `double exp(double x);`   | 指数函数 e^x            |
| `log`   | `double log(double x);`   | 自然对数（ln x）            |
| `log10` | `double log10(double x);` | 常用对数（log10 x）         |
| `log2`  | `double log2(double x);`  | 以 2 为底对数（C99+）       |

: 指数与对数函数（Exponential and Logarithmic Functions）{.striped}

| 函数名  | 原型                      | 描述               |
| ------- | ------------------------- | ------------------ |
| `sqrt`  | `double sqrt(double x);`            | 平方根        |
| `cbrt`  | `double cbrt(double x);`            | 立方根（C99+）     |

: 开方（Root Functions）{.striped}


| 函数名  | 原型                      | 描述               |
| ------- | ------------------------- | ------------------ |
| `ceil`  | `double ceil(double x);`  | 向上取整（到最近整数）     |
| `floor` | `double floor(double x);` | 向下取整（到最近整数）     |

: 取整与舍入函数（Rounding Functions）{.striped}

| 函数名  | 原型                      | 描述               |
| ------- | ------------------------- | ------------------ |
| `fabs`  | `double fabs(double x);` | 绝对值   |

: 绝对值函数（Absolute Value Function）{.striped}

::: important

C 语言的标准数学库函数主要定义在 `<math.h>` 头文件中。大多数函数返回 `double` 类型的值，并接受 `double` 类型参数。对于整数或浮点输入，通常通过类型提升转换为 `double`。

使用时需包含 `#include <math.h>`，并在链接时添加 `-lm`（Linux/Unix）。

:::

### 文法分析

给定的文法为（假设起始符号为 `expr`，终结符包括 `+`、`−`、`*`、`/`、`^`、`(`、`）`、`id`、`integer`、`float`，并引入结束符 `$`）：

- `expr ::= term { ( + | - ) term }`
- `term ::= unary { ( * | / ) unary }`
- `unary ::= ( + | - ) unary | power`
- `power ::= factor { ^ unary }`
- `factor ::= ( expr ) | id ( expr ) | id | integer | float`

该文法无 `ε`-产生式（空产生式），无左递归。以下逐步说明计算 FIRST 集和 FOLLOW 集的过程。计算基于标准算法：

#### FIRST 集计算步骤

FIRST(X) 表示从非终结符 X 导出的串的可能首终结符集合。算法：

1. 对于每个产生式 `A → α`（`α` 为右部符号序列），将 FIRST(`α` 的第一个符号)（减去 `ε`）加入 FIRST(A)。
2. 如果 `α` 的前缀可空（nullable），则继续向后符号添加 FIRST。
3. 迭代直到固定点（无变化）。
4. 重复和可选部分 `{ β }`（`β` 为非空）对 FIRST 无影响（因为可选且在首位后），只需关注首位符号的 FIRST。

- **FIRST(factor)**：产生式为 `( expr )`（首 `(`）、`id ( expr )`（首 `id`）、`id`（首 `id`）、`integer`（首 `integer`）、`float`（首 `float`）。 
   
  FIRST(factor) = `{ (, id, integer, float }`。

- **FIRST(power)**：`power → factor { ^ unary }`（首位 `factor`，可选部分不影响）。
    
  FIRST(power) = FIRST(factor) = `{ (, id, integer, float }`。

- **FIRST(unary)**：产生式为 `+ unary`（首 `+`）、`− unary`（首 `−`）、power（首位 power）。  
  
  FIRST(unary) = `{ +, − }` $\cup$ FIRST(power) = `{ +, −, (, id, integer, float }`。

- **FIRST(term)**：`term → unary { ( * | / ) unary }`（首位 unary）。 
   
  FIRST(term) = FIRST(unary) = `{ +, −, (, id, integer, float }`。

- **FIRST(expr)**：`expr → term { ( + | - ) term }`（首位 term）。
    
  FIRST(expr) = FIRST(term) = `{ +, −, (, id, integer, float }`。

#### FOLLOW 集计算步骤

FOLLOW(A) 表示可能紧跟在 A 后的终结符集合（包括 `$`）。算法：

1. FOLLOW(起始符号) $\supseteq$ `{ $ }`。
2. 对于每个产生式 `B → α A β`，将 FIRST(`β`)（减去 `ε`）加入 FOLLOW(A)；若 `β` 可空，则将 FOLLOW(B) 加入 FOLLOW(A)。
3. 对于可选/重复部分 `{ γ }`（`γ` 非空），在重复位置后添加 FIRST(`γ`) 和 FOLLOW(所在非终结符)。
4. 迭代直到固定点。

假设起始符号 `expr`，初始 FOLLOW(expr) = `{ $ }`。

- **FOLLOW(factor)**：factor 仅出现在 `power → factor { ^ unary }`。  
  `{ ^ unary }` 可空，故 FOLLOW(factor) $\supseteq$ FOLLOW(power)；同时 $\supseteq$ FIRST(`^` unary) = `{ ^ }`。 （后续计算 FOLLOW(power)，此处暂记）。

- **FOLLOW(power)**：power 仅出现在 `unary → power`，故 FOLLOW(power) $\supseteq$ FOLLOW(unary)。在 power 的重复 `{ ^ unary }` 中，unary 后为 `^` 或 power 结束，故 FOLLOW(unary) $\supseteq$  `{ ^ }` ∪ FOLLOW(power)。（循环依赖，后续统一）。

- **FOLLOW(unary)**：unary 出现在：  
  - `unary → + unary / − unary`：内层 unary 后为外层 unary，故 FOLLOW(unary) 传播自身。  
  - `term → unary { ( * | / ) unary }`：首位 unary 后可选部分可空，故 FOLLOW(unary) $\supseteq$ FOLLOW(term)；重复中 unary 后为 `* /` 或 term 结束，故 $\supseteq$ `{ *, / }` ∪ FOLLOW(term)。  
  - power 的重复中：如上，$\supseteq$ `{ ^ }` $\cup$ FOLLOW(power)。（后续计算 FOLLOW(term)）。

- **FOLLOW(term)**：term 出现在：  
  - `expr → term { ( + | - ) term }`：首位 term 后可选部分可空，故 FOLLOW(term) $\supseteq$ FOLLOW(expr)；重复中 term 后为 + − 或 expr 结束，故 $\supseteq$ `{ +, − }` ∪ FOLLOW(expr)。  
  （FOLLOW(expr) 已知）。

- **FOLLOW(expr)**：expr 出现在 factor → ( expr ) / id ( expr )，expr 后均为 `)`，故 FOLLOW(expr) $\supseteq$ `{ ) }`。结合起始，FOLLOW(expr) = `{ $, ) }`。

现在迭代传播：  

- FOLLOW(term) = `{ +, − }` ∪ FOLLOW(expr) = `{ +, −, $, ) }`。  
- FOLLOW(unary) = `{ *, / }` ∪ `{ ^ }` ∪ FOLLOW(term) = `{ *, /, ^, +, −, $, ) }`。  
- FOLLOW(power) = FOLLOW(unary) = `{ +, −, *, /, ^, $, ) }`。  
- FOLLOW(factor) = `{ ^ }` ∪ FOLLOW(power) = `{ +, −, *, /, ^, $, ) }`（^ 已包含）。

#### 结果汇总

| 非终结符 | FIRST 集                          | FOLLOW 集                 |
| -------- | --------------------------------- | ------------------------- |
| expr     | `{ +, −, (, id, integer, float }` | `{ $, ) }`                |
| term     | `{ +, −, (, id, integer, float }` | `{ +, −, $, ) }`          |
| unary    | `{ +, −, (, id, integer, float }` | `{ +, −, *, /, ^, $, ) }` |
| power    | `{ (, id, integer, float }`       | `{ +, −, *, /, ^, $, ) }` |
| factor   | `{ (, id, integer, float }`       | `{ +, −, *, /, ^, $, ) }` |

: 文法非终结符的 FIRST 和 FOLLOW 集

该文法满足 LL(1) 条件（FIRST 集两两不相交，FOLLOW 中无冲突），可用于自顶向下解析。