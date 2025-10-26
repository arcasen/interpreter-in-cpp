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
#include "AbstractSyntaxTree.h"
#include <stdlib.h>

void Interpreter::visit(BinaryExpression *binexp)
{
    binexp->children[0]->accept(this);
    double a = ans;
    binexp->children[1]->accept(this);
    double b = ans;

    if (binexp->token->text == "+")
        ans = a + b;
    else if (binexp->token->text == "-")
        ans = a - b;
    else
        throw "Undefined operation!";
}

void Interpreter::visit(NumberLiteral *integer)
{
    ans = atof(integer->token->text.c_str());
}
