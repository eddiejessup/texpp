/* This file is part of texpp.
   Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

   texpp is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   texpp is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with texpp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <texpp/base/func.h>
#include <texpp/parser.h>

namespace texpp {
namespace base {

Node::ptr Let::parse(Parser& parser)
{
    Node::ptr node(new Node(name()));

    node->appendChild("command", parser.parseControlSequence());
    node->appendChild("lvalue", parser.parseControlSequence());
    node->appendChild("equals", parser.parseOptionalEquals(true));
    node->appendChild("rvalue", parser.parseToken());

    return node;
}

bool Let::execute(Parser& parser, Node::ptr node)
{
    parser.setSymbol(
        node->child(1)->value(Token::ptr()),
        parser.symbol(
            node->child(3)->value(Token::ptr()),
            Command::ptr()
        )
    );
    return true;
}

Node::ptr FutureLet::parse(Parser& parser)
{
    Node::ptr node(new Node(name()));

    node->appendChild("command", parser.parseControlSequence());
    node->appendChild("lvalue", parser.parseControlSequence());
    node->appendChild("token", parser.parseOptionalEquals(true));
    node->appendChild("rvalue", parser.parseToken());

    // XXX TODO: push tokens back

    return node;
}

bool FutureLet::execute(Parser& parser, Node::ptr node)
{
    parser.setSymbol(
        node->child(1)->value(Token::ptr()),
        parser.symbol(
            node->child(3)->value(Token::ptr()),
            Command::ptr()
        )
    );
    return true;
}


} // namespace base
} // namespace texpp

