# Implementation of an Interpreter

## Introduction

A C/C++ implementation of interpreter for the bellowing syntax:

```
expr    ::= term { ( + | - ) term }
term    ::= unary { ( * | / ) unary }
unary   ::= ( + | - ) unary | power
power   ::= factor { '^' unary }  
factor  ::= ( expr ) | integer | float
```

## References
1. [Robert Nystrom, Crafting Interpreters](https://craftinginterpreters.com/)
2. [Charles N. Fischer et al, Crafting a Compiler, 2009](https://www.pearsonhighered.com/program/Fischer-Crafting-A-Compiler/PGM315544.html)
3. [Ronald Mak, Writing Compilers and Interpreters: A Software Engineering Approach, 3rd Edition](https://www.amazon.com/Writing-Compilers-Interpreters-Software-Engineering/dp/0470177071)
4. [Terence Parr, Language Implementation Patterns: Create Your Own Domain-Specific and General Programming Languages, 2010](https://pragprog.com/book/tpdsl/language-implementation-patterns)
5. [Keith D. Cooper et al, Engineering a Compiler, 2nd Edition](http://www.cs.rice.edu/~keith/)
6. [Luna Programming Language](https://github.com/tj/luna)
