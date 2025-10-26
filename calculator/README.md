# Calculator

## Grammar

参考
https://docs.python.org/3/reference/grammar.html

```
expr   ::= term { ( + | - ) term }
term   ::= unary { ( * | / ) unary }
unary  ::= ( + | - ) unary | power
power  ::= factor { ^ unary }  
factor ::= ( expr ) | id ( expr ) | id | integer | float
id     ::= [a-zA-Z]+[0-9]*
```
