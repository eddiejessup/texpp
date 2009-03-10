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

bool Let::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("lvalue", parser.parseControlSequence());
    node->appendChild("equals", parser.parseOptionalEquals(true));
    node->appendChild("rvalue", parser.parseToken());
    return true;
}

bool Let::execute(Parser& parser, Node::ptr node)
{
    Token::ptr token = node->child("rvalue")->value(Token::ptr());
    if(token->isControl()) {
        parser.setSymbol(
            node->child("lvalue")->value(Token::ptr()),
            parser.symbol(token, Command::ptr())
            );
    } else {
        parser.setSymbol(
            node->child("lvalue")->value(Token::ptr()),
            Command::ptr(new TokenCommand(token)));
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
        )
    );
    return true;
}


} // namespace base
} // namespace texpp

