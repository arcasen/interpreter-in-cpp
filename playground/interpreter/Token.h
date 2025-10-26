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

#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>

struct Token {

    enum TokenType { None = 256,
        Number = 257,
        Plus = 258,
        Minus = 259 };

    Token()
        : text("")
        , row(0)
        , column(0)
        , type(None)
    {
    }

    Token(const std::string &text, int row, int column, int type)
        : text(text)
        , row(row)
        , column(column)
        , type(type)
    {
    }

    ~Token()
    {
        std::cout << ".. Deleting token: " << text << std::endl;
    }

    int type;
    std::string text; // token string
    int row; // line number
    int column; // column number
};

#endif /* TOKEN_H */
