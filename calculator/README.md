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

## Compile

GCC/Clang:

```
mkdir build
cd build
cmake ..
make
```

MSVC:

```bash
mkdir build
cd build
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
vcvarsall.bat
nmake
```