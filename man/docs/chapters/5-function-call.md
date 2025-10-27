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