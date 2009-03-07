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

