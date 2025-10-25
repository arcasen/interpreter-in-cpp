# Calculator

## Grammar

```
expr   ::= term { ( + | - ) term }
term   ::= factor { ( * | / ) factor }
factor ::= ( expr ) | integer
integer ::= [0-9]+
```

## Build

Release build:

```
make
```

Debug build:

```
make debug
```

## Usage

```
./calc "21-(4+5)*2"
```