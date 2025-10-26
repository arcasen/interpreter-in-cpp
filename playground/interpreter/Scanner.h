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

#ifndef SCANNER_H
#define SCANNER_H

#include "CharStream.h"
#include "Token.h"
#include <memory>

class Scanner {
public:
    explicit Scanner(CharStream *charStream)
        : charStream(charStream)
    {
        nextToken();
    }

    std::shared_ptr<Token> currentToken() const
    {
        return token;
    }

    std::shared_ptr<Token> nextToken();

    // Call charStream's currentChar() method
    char currentChar() const
    {
        return charStream->currentChar();
    }

    // Call charStream's currentChar() method
    char nextChar()
    {
        return charStream->nextChar();
    }

    bool isWhiteSpace(char ch) const
    {
        return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
    }

    void skipWhiteSpace()
    {
        while (isWhiteSpace(currentChar()))
            nextChar();
    }

private:
    // Initialize token pointer
    void initToken();
    // Set current token's text
    void setText(std::string text);
    // Set current token's type
    void setType(int type);
    // Append current char to current token's text
    void enterChar();
    // Scanning unsigned integer literal
    bool integerLiteral();
    // Scanning unsigned number literal
    bool numberLiteral();

    CharStream *charStream; // source code
    std::shared_ptr<Token> token; // current token
};

#endif /* SCANNER_H */
