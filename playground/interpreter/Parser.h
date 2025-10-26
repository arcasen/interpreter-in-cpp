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

#ifndef PARSER_H
#define PARSER_H

#include "AbstractSyntaxTree.h"
#include "Scanner.h"

class Parser {
public:
    explicit Parser(Scanner *scanner)
        : scanner(scanner)
    {
    }

    std::shared_ptr<AbstractNode> expression();
    std::shared_ptr<AbstractNode> number();

private:
    std::shared_ptr<Token> currentToken() const
    {
        return scanner->currentToken();
    }

    std::shared_ptr<Token> nextToken()
    {
        return scanner->nextToken();
    }

    bool match(std::shared_ptr<Token> token, int type)
    {
        if (token->type != type)
            return false;
        nextToken();
        return true;
    }

    Scanner *scanner; // from where we get tokens
};

#endif /* PARSER_H */
