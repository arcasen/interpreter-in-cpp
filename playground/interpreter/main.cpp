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

#include "Interpreter.h"
#include "Parser.h"
#include <iostream>

// Input examples:
// 135 + 24 - 8     // valid input
// 135 + 24 - 8 8   // unexpected integer 8
// 135 + 24 - 8 +   // expecting an integer
int main(int argc, char **argv)
{
    for (;;) {
        try {
            std::cout << "> ";

            std::string input;
            std::cin >> input;

            // On Unix-like OS, when press Ctrl+D
            if (input.empty())
                break;

            CharStream charStream(input);
            Scanner scanner(&charStream);
            Parser parser(&scanner);

            auto ast = parser.expression();

            Interpreter interpreter;
            ast.get()->accept(&interpreter);
            std::cout << interpreter.answer() << std::endl;

        } catch (const char *msg) {
            std::cout << msg << std::endl;
        }
    }
    return 0;
}
