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

#ifndef VISITOR_PATTERN_H
#define VISITOR_PATTERN_H

#define ABSTRACT_VISIT_METHOD_DECL(classname) \
    virtual void visit(classname *) = 0;

#define CONCRETE_VISIT_METHOD_DECL(classname) \
    void visit(classname *);

#define ABSTRACT_ACCEPT_METHOD_DECL \
    virtual void accept(Visitor *visitor) = 0;

#define CONCRETE_ACCEPT_METHOD_DECL \
    void accept(Visitor *visitor);

#define CONCRETE_ACCEPT_METHOD_IMPL(classname) \
    void classname::accept(Visitor *visitor)   \
    {                                          \
        return visitor->visit(this);           \
    }

class BinaryExpression;
class NumberLiteral;

class Visitor {
public:
    ABSTRACT_VISIT_METHOD_DECL(BinaryExpression)
    ABSTRACT_VISIT_METHOD_DECL(NumberLiteral)
};

#endif /* VISITOR_PATTERN_H */
