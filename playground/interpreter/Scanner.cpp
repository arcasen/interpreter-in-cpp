// Copyright (C) 2015-2016, kylinsage <kylinsage@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Scanner.h"
#include <iostream>

std::shared_ptr<Token> Scanner::nextToken()
{
    skipWhiteSpace();
    initToken();

    char ch;
    if ((ch = currentChar()) != EOF) {
        switch (ch) {
        case '+':
            setType(Token::Plus);
            enterChar();
            nextChar();
            break;
        case '-':
            setType(Token::Minus);
            enterChar();
            nextChar();
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
            numberLiteral();
            break;
        default:
            throw "Invalid character!";
        }
    } else {
        setType(EOF);
        setText("EOF");
    }
    std::cout << ".. Scanning token: " << token->text
              << ", type: " << token->type << std::endl;
    return token;
}

void Scanner::initToken()
{
    token = std::make_shared<Token>("",
        charStream->currentRow(),
        charStream->currentColumn(),
        Token::None);
}

void Scanner::enterChar()
{
    token->text.append(1, currentChar());
}

// Set current token text
void Scanner::setText(std::string text)
{
    token->text = text;
}

// Set current token type
void Scanner::setType(int type)
{
    token->type = type;
}

// Scanning unsigned integer
// Return true on success, otherwise, return false
bool Scanner::integerLiteral()
{
    if (!isdigit(currentChar()))
        return false;

    while (isdigit(currentChar())) {
        enterChar();
        nextChar();
    }
    return true;
}

// Scanning unsigned number:
// number ::= ( integer ( '.' integer? )? | '.' integer ) ( ( 'e' | 'E' ) ( '+' | '-' )? integer )?
// Return true on success, otherwise, return false
bool Scanner::numberLiteral()
{
    setType(Token::Number); /* default type */

    if (isdigit(currentChar())) {
        integerLiteral();
        if (currentChar() == '.') {
            //setType(Float);
            enterChar();
            nextChar();
            if (isdigit(currentChar())) {
                integerLiteral();
            }
        }
    } else if (currentChar() == '.') {
        //setType(Float);
        enterChar();
        nextChar();
        if (!integerLiteral())
            return false;
    } else
        return false;

    if (currentChar() == 'e' || currentChar() == 'E') {
        //setType(Float);
        enterChar();
        nextChar();
        if (currentChar() == '+' || currentChar() == '-') {
            enterChar();
            nextChar();
        }
        if (!integerLiteral())
            return false;
    }

    return true;
}
