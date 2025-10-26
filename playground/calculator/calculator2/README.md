# Calculator

## Grammar

```
expr    ::= term { ( + | - ) term }
term    ::= unary { ( * | / ) unary }
unary   ::= ( + | - ) unary | factor
factor  ::= ( expr ) | integer
integer ::= [0-9]+
```