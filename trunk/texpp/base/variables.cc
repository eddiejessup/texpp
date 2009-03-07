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

#include <texpp/base/variables.h>
#include <texpp/parser.h>

#include <iostream>

namespace texpp {
namespace base {

const any& Variable::get(Parser& parser, bool global)
{
    return parser.symbolAny(name().substr(1), global);
}

bool Variable::set(Parser& parser, const any& value, bool global)
{
    string varname = name().substr(1);
    any s = parser.symbolAny(varname, global);
    if(s.type() == value.type()) {
        parser.setSymbol(varname, value, global);
        return true;
    } else {
        return false;
    }
}

Node::ptr IntegerVariable::parse(Parser& parser)
{
    Node::ptr node(new Node(name()));
    node->appendChild("command", parser.parseControlSequence());
    node->appendChild("equals", parser.parseOptionalEquals(false));
    node->appendChild("rvalue", parser.parseNumber());

    return node;
}

bool IntegerVariable::execute(Parser& parser, Node::ptr node)
{
    std::cout << "IntegerVariable:" << node->child(2)->value(int(0)) << std::endl;
    parser.setSymbol(name().substr(1),
                node->child(2)->value(int(0)));
    return true;
}


} // namespace base
} // namespace texpp

