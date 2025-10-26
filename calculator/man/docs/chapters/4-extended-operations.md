## 实现一元运算和乘方运算

### 浮点数分析

C 语言（C11/C17/C23 标准，§6.4.4.2）中的十进制浮点常量的正则表达式如下：

```
^[+-]?(?:(?:\d+\.\d*|\.\d+)(?:[eE][+-]?\d+)?|\d+[eE][+-]?\d+)(?:[fFlL])?$
```

考虑到编写词法分析器比较费时，直接使用库函数 `strtod` 来读取浮点数。

### 增加一元运算和乘方运算的文法

参考 Python 语法[^python-grammar]将文法改写为：

[^python-grammar]: <https://docs.python.org/3/reference/grammar.html>

```ebnf
expr   ::= term { ( + | - ) term }
term   ::= unary { ( * | / ) unary }
unary  ::= ( + | - ) unary | power
power  ::= factor { '^' unary }  # 一元在这里处理
factor ::= ( expr ) | integer | float
```

::: important

要特别注意运算的优先级和结合顺序（乘方运算是右结合的），如果文法不正确将导致表达式解析错误或计算顺序错误，如 `-3^2`， `2^3^4`。

:::