/* Copyright (C) 2017, kylinsage <kylinsage@gmail.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /***********************************************************
  * Standalone parser for bellowing syntax:
  * expression ::= number ( ( '+' | '-' ) number )*
  * number ::= ( integer ( '.' integer? )? | '.' integer ) ( ( 'e' | 'E' ) ( '+' | '-' )? integer )?
  * integer ::= ( '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' )+
  *
  * Compile:
  *     gcc parser.c
  * Run:
  *     ./a.out < example1.txt
  *     ./a.out  example1.txt example2.txt ...
  * or, run it interactively (press Enter to input another line, and Ctrl+D to
  * finish):
  *     ./a.out
  *     >>> 3-4
  *     >>> 3+4-
  *     ... 5
  *     >>>
  **********************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "error.h"
#include "exception.h"

#define EOL -1

/***********************************************************
 * stdin_is_tty detects whether the standard input is a 'tty' (that
 * is, whether we're running parser interactively).
 ***********************************************************/
/* Microsoft Windows */
#if defined(_WIN32)
#include <io.h>
#include <windows.h>
#define stdin_is_tty()	_isatty(_fileno(stdin))
/* UNIX */
#elif defined(__unix__) || defined(__APPLE__) && defined(__MACH__)
#include <unistd.h>
#define stdin_is_tty()	isatty(0)
/* ISO C definition */
#else
#define stdin_is_tty()	1       /* assume stdin is a tty */
#endif

static int is_interactive = 1;

/* prompt of the first line */
static inline void initial_prompt()
{
    printf(">>> ");
}

/* prompt of the following line */
static inline void multiline_prompt()
{
    printf("... ");
}

/***********************************************************
 * CharStream -- characters stream
 **********************************************************/
struct CharStream {
    FILE *fp;                   /* source code file */
    char buffer[2048];          /* source code */
    int row;                    /* current line number */
    int column;                 /* current line position, start from 1 */
    char ch;                    /* current character */
};

static inline int charstream_read_line(struct CharStream *charStream)
{
    charStream->column = 1;
    if (fgets(charStream->buffer, 2048, charStream->fp)) {
        charStream->ch = charStream->buffer[charStream->column - 1];
        return 1;
    } else {
        charStream->ch = EOL;
        return 0;
    }
}

/* Initialize characters stream */
int charstream_init(struct CharStream *charStream, FILE * fp)
{
    charStream->fp = fp;
    charStream->row = 1;
    if (is_interactive)
        initial_prompt();
    return charstream_read_line(charStream);
}

/* If previous line is not a complete expression, then read the next line */
int charstream_next_line(struct CharStream *charStream)
{
    charStream->row++;
    if (is_interactive)
        multiline_prompt();
    return charstream_read_line(charStream);
}

/* Return the source character at the current position. */
char charstream_current_char(struct CharStream *charStream)
{
    return charStream->ch;
}

/* Consume the current source character and return the next character. */
char charstream_next_char(struct CharStream *charStream)
{
    if (charStream->column == strlen(charStream->buffer)) {
        charStream->ch = -1;
        return EOL;
    }
    charStream->column++;
    charStream->ch = charStream->buffer[charStream->column - 1];
    return charStream->ch;
}

/***********************************************************
 * Scanner -- generating tokens stream
 **********************************************************/
/* Token types */
#define DECL_TOKEN_SET \
    DECL_TOKEN(None, "none") \
    DECL_TOKEN(EOS, "<end>") \
    DECL_TOKEN(Integer, "<integer>") \
    DECL_TOKEN(Float, "<float>") \
    DECL_TOKEN(Plus, "+") \
    DECL_TOKEN(Minus, "-")

#define DECL_TOKEN(type, text) type,
typedef enum {
    DECL_TOKEN_SET
} TokenType;
#undef DECL_TOKEN

#define DECL_TOKEN(type, text) text,
static const char *TokenText[] = {
    DECL_TOKEN_SET
};

#undef DECL_TOKEN
//int None = 256, Integer = 257, Float = 258, Plus = 259, Minus = 260;

struct Token {
    char text[64];              /* token string */
    int row;                    /* line number */
    int column;                 /* column number */
    int type;                   /* token type */
};

struct Scanner {
    struct CharStream *charStream;  /* source code */
    struct Token token;         /* current token */
};

/* Call struct CharStream's method */
char scanner_current_char(struct Scanner *scanner)
{
    return charstream_current_char(scanner->charStream);
}

/* Call struct CharStream's method */
char scanner_next_char(struct Scanner *scanner)
{
    return charstream_next_char(scanner->charStream);
}

/* Returns current token */
struct Token scanner_current_token(struct Scanner *scanner)
{
    return scanner->token;
}

/* Check whether a character is white space */
int isWhiteSpace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

void scanner_skip_whitespace(struct Scanner *scanner)
{
    while (isWhiteSpace(scanner_current_char(scanner))) {
        /*if (scanner_current_char(scanner) == '\n') {
           scanner->charStream->row++;
           scanner->charStream->column = 0;
           } */
        scanner_next_char(scanner);
    }
}

/* Reset current token */
void scanner_init_token(struct Scanner *scanner)
{
    memset(scanner->token.text, 0, 64);
    scanner->token.row = scanner->charStream->row;
    scanner->token.column = scanner->charStream->column;
    scanner->token.type = None;
}

/* Append current character to current token's text */
void scanner_enter_char(struct Scanner *scanner)
{
    int n = strlen(scanner->token.text);
    if (n >= 63)
        return;
    scanner->token.text[n] = scanner->charStream->ch;
}

/* Set current token text */
void scanner_set_text(struct Scanner *scanner, char *text)
{
    strncpy(scanner->token.text, text, 64);
}

/* Set current token type */
void scanner_set_type(struct Scanner *scanner, int type)
{
    scanner->token.type = type;
}

/* Scanning unsigned integer */
void scanner_integer_literal(struct Scanner *scanner)
{
    if (!isdigit(scanner_current_char(scanner)))
        raise_exception(INVALID_NUMBER, "Invalid number!");

    while (isdigit(scanner_current_char(scanner))) {
        scanner_enter_char(scanner);
        scanner_next_char(scanner);
    }
}

/* Scanning unsigned number:
   number ::= ( integer ( '.' integer? )? | '.' integer ) ( ( 'e' | 'E' ) ( '+' | '-' )? integer )?
 */
void scanner_numeric_literal(struct Scanner *scanner)
{
    scanner_set_type(scanner, Integer); /* default type */

    if (isdigit(scanner_current_char(scanner))) {
        scanner_integer_literal(scanner);
        if (scanner_current_char(scanner) == '.') {
            scanner_set_type(scanner, Float);
            scanner_enter_char(scanner);
            scanner_next_char(scanner);
            if (isdigit(scanner_current_char(scanner))) {
                scanner_integer_literal(scanner);
            }
        }
    } else if (scanner_current_char(scanner) == '.') {
        scanner_set_type(scanner, Float);
        scanner_enter_char(scanner);
        scanner_next_char(scanner);
        scanner_integer_literal(scanner);
    } else
        raise_exception(INVALID_NUMBER, "Invalid number!");

    if (scanner_current_char(scanner) == 'e'
        || scanner_current_char(scanner) == 'E') {
        scanner_set_type(scanner, Float);
        scanner_enter_char(scanner);
        scanner_next_char(scanner);
        if (scanner_current_char(scanner) == '+' ||
            scanner_current_char(scanner) == '-') {
            scanner_enter_char(scanner);
            scanner_next_char(scanner);
        }
        scanner_integer_literal(scanner);
    }
}

/* Consume the current token and return the next token . */
struct Token scanner_next_token(struct Scanner *scanner)
{
    scanner_skip_whitespace(scanner);
    scanner_init_token(scanner);

    char ch;
    if ((ch = scanner_current_char(scanner)) != EOL) {
        switch (ch) {
        case '+':
            scanner_set_type(scanner, Plus);
            scanner_enter_char(scanner);
            scanner_next_char(scanner);
            break;
        case '-':
            scanner_set_type(scanner, Minus);
            scanner_enter_char(scanner);
            scanner_next_char(scanner);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            scanner_numeric_literal(scanner);
            break;
        default:
            raise_exception(INVALID_CHARACTER, "Invalid character!");
        }
    } else {
        scanner_set_text(scanner, "EOS");
        scanner_set_type(scanner, EOS);
    }
    printf(".. Scanning token: %s, position: (%d, %d), type: %s\n",
           scanner->token.text,
           scanner->token.row, scanner->token.column,
           TokenText[scanner->token.type]);
    return scanner->token;
}

/* Initialize a scanner */
void scanner_init(struct Scanner *scanner, struct CharStream *charStream)
{
    scanner->charStream = charStream;
    scanner_next_token(scanner);
}

/***********************************************************
 * Parser
 **********************************************************/
struct Parser {
    struct Scanner *scanner;    // from where we get tokens
};

/* Initialize a parser */
void parser_init(struct Parser *parser, struct Scanner *scanner)
{
    parser->scanner = scanner;
}

/* Call struct Scanner's method */
struct Token parser_current_token(struct Parser *parser)
{
    return scanner_current_token(parser->scanner);
}

/* Call struct Scanner's method */
struct Token parser_next_token(struct Parser *parser)
{
    return scanner_next_token(parser->scanner);
}

/* Check whether the current token matches the specific type */
void parser_match(struct Parser *parser, int type)
{
    if (parser->scanner->token.type == EOS && type != EOS)
        raise_exception(SYNTAX_ERROR, "Incomplete!");

    if (parser->scanner->token.type != type)
        raise_exception(SYNTAX_ERROR, "Token type not matched!");
    if (type != EOS)
        parser_next_token(parser);
}

/* Parsing number */
void parser_number(struct Parser *parser)
{
    if (parser_current_token(parser).type == Integer)
        parser_match(parser, Integer);
    else if (parser_current_token(parser).type == Float)
        parser_match(parser, Float);
    else if (parser_current_token(parser).type == EOS &&
             charstream_next_line(parser->scanner->charStream)) {
        parser_next_token(parser);
        parser_number(parser);

    } else
        raise_exception(SYNTAX_ERROR, "Expect a number!");
}

/* Parsing expression */
void parser_expression(struct Parser *parser)
{
    parser_number(parser);

    while (parser_current_token(parser).type == Plus ||
           parser_current_token(parser).type == Minus) {
        parser_next_token(parser);
        parser_number(parser);
    }

    parser_match(parser, EOS);
}

int parse(FILE * fp)
{
    struct CharStream charStream;
    struct Scanner scanner;
    struct Parser parser;

    /* Initializations */
    if (!charstream_init(&charStream, fp))
        return 0;

    try {
        scanner_init(&scanner, &charStream);    /* throws */
        parser_init(&parser, &scanner);
        parser_expression(&parser);
        printf("Accepted!\n");
    }
    finally {
        printf("Syntax error, code: %d!\n", _except_code_);
    }

    return 1;
}

/* An Interactive Multiline Commands REPL */
int main(int argc, char *argv[])
{
    if (argc == 1 && stdin_is_tty()) {
        is_interactive = 1;
        do {
        } while (parse(stdin));
    } else if (argc == 1) {
        is_interactive = 0;
        printf("## Parsing stdin:\n");
        parse(stdin);
    } else {
        is_interactive = 0;
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp)
                continue;
            printf("## Parsing %s:\n", argv[i]);
            parse(fp);
            fclose(fp);
        }
    }

    return 0;
}
