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

bool Prefix::invoke(Parser& parser, shared_ptr<Node>)
{
    parser.activePrefixes().insert(name());
    return true;
}

bool Let::invoke(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr lvalue = parser.parseControlSequence();
    node->appendChild("lvalue", lvalue);
    node->appendChild("equals", parser.parseOptionalEquals(true));

    Node::ptr rvalue = parser.parseToken();
    node->appendChild("rvalue", rvalue);

    Token::ptr ltoken = lvalue->value(Token::ptr());
    Token::ptr rtoken = rvalue->value(Token::ptr());

    if(rtoken->isControl()) {
        parser.setSymbol(
            ltoken, parser.symbol(rtoken, Command::ptr()),
            parser.isPrefixActive("\\global")
            );
    } else {
        parser.setSymbol(
            ltoken, Command::ptr(new TokenCommand(rtoken)),
            parser.isPrefixActive("\\global")
            );
    }

    return true;
}

bool FutureLet::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("lvalue", parser.parseControlSequence());
    node->appendChild("token", parser.parseOptionalEquals(true));
    node->appendChild("rvalue", parser.parseToken());

    // XXX TODO: push tokens back

    return true;
}

bool FutureLet::execute(Parser& parser, Node::ptr node)
{
    parser.setSymbol(
        node->child(1)->value(Token::ptr()),
        parser.symbol(
            node->child(3)->value(Token::ptr()),
            Command::ptr()
        ),
        parser.isPrefixActive("\\global")
    );
    return true;
}


} // namespace base
} // namespace texpp

