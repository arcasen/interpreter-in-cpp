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

#include "Parser.h"
#include <iostream>

std::shared_ptr<AbstractNode> Parser::number()
{
    auto token = currentToken();
    if (match(token, Token::Number))
        return std::make_shared<NumberLiteral>(token);
    else
        throw "SyntaxError: number is expected!";
}

std::shared_ptr<AbstractNode> Parser::expression()
{
    auto root = number();

    while (currentToken()->type == Token::Plus || currentToken()->type == Token::Minus) {
        auto token = currentToken();
        nextToken();
        auto lhs = root;
        auto rhs = number();
        root = std::make_shared<BinaryExpression>(token);
        root->addChild(lhs);
        root->addChild(rhs);
    }

    if (currentToken()->type == EOF)
        printf("Accepted!\n");
    else
        throw "SyntaxError: unexpected token!";

    return root;
}
