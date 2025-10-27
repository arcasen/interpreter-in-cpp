# Implementation of an Expression Calculator

## Introduction

A C/C++ implementation of calculator (REPL) for the bellowing syntax:

```
expr   ::= term { ( + | - ) term }
term   ::= unary { ( * | / ) unary }
unary  ::= ( + | - ) unary | power
power  ::= factor { ^ unary }  
factor ::= ( expr ) | id ( expr ) | id | integer | float
```

## Usage

Run calculator and input expression in REPL:

```
 ./calc
Welcome to the expression calculator.
Enter 'exit' to exit.
> 1-2
ans: -1
> sin(pi/4)
ans: 0.707107
> exit
Bye.
```

## Features

- Built-in constants: `pi`, `e`, `phi`, `ans`.
- Built-in functions: 
  - Trigonometric functions: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`.
  - Hyperbolic functions: `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`.
  - Exponential and logarithmic functions: `exp`, `log`, `log10`, `log2`.
  - Root functions: `sqrt`, `cbrt`.
  - Rounding functions: `ceil`, `floor`.
  - Absolute value function: `fabs`.

## References
1. [Robert Nystrom, Crafting Interpreters](https://craftinginterpreters.com/)
2. [Charles N. Fischer et al, Crafting a Compiler, 2009](https://www.pearsonhighered.com/program/Fischer-Crafting-A-Compiler/PGM315544.html)
3. [Ronald Mak, Writing Compilers and Interpreters: A Software Engineering Approach, 3rd Edition](https://www.amazon.com/Writing-Compilers-Interpreters-Software-Engineering/dp/0470177071)
4. [Terence Parr, Language Implementation Patterns: Create Your Own Domain-Specific and General Programming Languages, 2010](https://pragprog.com/book/tpdsl/language-implementation-patterns)
5. [Keith D. Cooper et al, Engineering a Compiler, 2nd Edition](http://www.cs.rice.edu/~keith/)
6. [Luna Programming Language](https://github.com/tj/luna)
