/*  This file is part of texpp library.
    Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <texpp/base/arithmetic.h>
#include <texpp/base/integer.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {
namespace base {

bool ArithmeticCommand::parseArgs(Parser& parser, Node::ptr node)
{
    Node::ptr arg(new Node("lvalue"));
    shared_ptr<IntegerVariable> integer =
        parser.parseCommandOrGroup<IntegerVariable>(arg);
    if(integer) {
        // TODO: optional by
        arg->setType("integer_variable");
        arg->setValue(integer);
        node->appendChild("lvalue", arg);
        node->appendChild("rvalue", parser.parseNumber());
        return true;
    }

    parser.logger()->log(Logger::ERROR,
        string("You can't use `") + parser.peekToken()->texRepr() +
        string("' after ") + texRepr(),
        parser, parser.lastToken());
    node->setValue(int(0));

    node->appendChild("internal_quantity", parser.parseToken());
    node->child("internal_quantity")->setValue(int(0));

    return true;
}

bool ArithmeticCommand::execute(Parser& parser, Node::ptr node)
{
    Node::ptr arg = node->child("lvalue");
    if(arg->type() == "integer_variable") {
        shared_ptr<IntegerVariable> integer =
            arg->value(shared_ptr<IntegerVariable>());
        int v1 = integer->get(parser, int(0));
        int v2 = node->child("rvalue")->value(int(0));
        if(m_type == ADVANCE) v1 += v2;
        else if(m_type == MULTIPLY) v1 *= v2;
        else if(m_type == DIVIDE) v1 /= v2;
        integer->set(parser, v1);
    }

    return true;
}

} // namespace base
} // namespace texpp

