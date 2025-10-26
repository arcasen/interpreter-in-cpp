# Multiline Statements REPL

When you enter an expression, the parser tries to parse, and if it finds that the expression does not end, then the parser will wait for you to continue typing until you end the input (Ctrl+D) or encounter an syntax error.

For example:

```
>>> 3-4+
... 5
>>>
```
