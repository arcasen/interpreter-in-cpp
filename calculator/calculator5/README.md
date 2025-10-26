# Calculator

参考
https://docs.python.org/3/reference/grammar.html

## Grammar

expr    ::= term { ( + | - ) term }
term    ::= unary { ( * | / ) unary }
unary   ::= ( + | - ) unary | power
power   ::= factor { '^' unary }  # 一元在这里处理
factor  ::= ( expr ) | integer | float


Token list 采用双向链表
因为解析负数时需要向前看 2 个 Token

判断 +/- 为正负号: （^ 表示开始， $表示结尾）

- 下一个 token 为 integer
- 前一个 token 为 ^(NULL） ( + - * / 

浮点数借助 strtod 库函数

由于在解析 -3^2 这样的式子是 - 不能与 3 结合, 不能读出 -3 
在文法中去掉正负号的处理 (sign)
把正负号当做一元运算处理
