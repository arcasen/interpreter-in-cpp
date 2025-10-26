# Calculator

## Grammar

```
expr    ::= term { ( + | - ) term }
term    ::= factor { ( * | / ) factor }
factor  ::= ( expr ) | integer
integer ::= [0-9]+
```