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

#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include "Token.h"
#include "VisitorPattern.h"
#include <iostream>
#include <memory>
#include <vector>

class AbstractNode {
public:
    ABSTRACT_ACCEPT_METHOD_DECL

    AbstractNode(std::shared_ptr<Token> token)
        : token(token)
    {
        std::cout << ".. Creating AST node: " << token->text << std::endl;
    }

    virtual ~AbstractNode()
    {
        std::cout << ".. Deleting AST node: " << token->text << std::endl;
    }

    void addChild(std::shared_ptr<AbstractNode> child)
    {
        children.push_back(child);
    }

    std::shared_ptr<Token> token;
    std::vector<std::shared_ptr<AbstractNode>> children;
};

class BinaryExpression : public AbstractNode {
public:
    CONCRETE_ACCEPT_METHOD_DECL

    BinaryExpression(std::shared_ptr<Token> token)
        : AbstractNode(token)
    {
    }

    ~BinaryExpression() {}
};

class NumberLiteral : public AbstractNode {
public:
    CONCRETE_ACCEPT_METHOD_DECL

    NumberLiteral(std::shared_ptr<Token> token)
        : AbstractNode(token)
    {
    }

    ~NumberLiteral() {}
};

#endif /* ABSTRACT_SYNTAX_TREE_H */
