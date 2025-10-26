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

#ifndef CHARSTREAM_H
#define CHARSTREAM_H

#include <string>
using std::string;

class CharStream {
public:
    explicit CharStream(const string &text)
        : text(text)
        , row(1)
        , column(1)
    {
    }

    int currentRow()
    {
        return row;
    }

    int currentColumn()
    {
        return column;
    }

    // Return the source character at the current position.
    char currentChar() const;

    // Consume the current source character and return the next character.
    char nextChar()
    {
        ++column;
        return currentChar();
    }

private:
    string text; // source line
    int row; // current line number
    int column; // current line position, start from 1
};

#endif /* CHARSTREAM_H */
