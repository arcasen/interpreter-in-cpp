# Calculator

## Grammar

```
expr    ::= term { ( + | - ) term }
term    ::= unary { ( * | / ) unary }
unary   ::= ( + | - ) unary | factor
factor  ::= ( expr ) | integer | float
float   ::= sign? ( (digit+ ( . digit* )? ) | ( . digit+ ) ) ( e sign? digit+ )?
integer ::= sign? digit+
digit   ::= [0-9]
sign    ::= [+-]
```

Token list 采用双向链表
因为解析负数时需要向前看 2 个 Token

判断 +/- 为正负号: （^ 表示开始， $表示结尾）

- 下一个 token 为 integer
- 前一个 token 为 ^(NULL） ( + - * / 

浮点数借助 strtod 库函数