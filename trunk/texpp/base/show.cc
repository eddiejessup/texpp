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

#include <texpp/base/show.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool Show::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("token", parser.parseToken());
    return true;
}

bool Show::execute(Parser& parser, Node::ptr node)
{
    string str;
    Token::ptr token = node->child("token")->value(Token::ptr());
    if(token->isCharacter()) {
        str = token->meaning();
    } else {
        char escape = parser.symbol("escapechar", int(0));
        str = token->texRepr(escape) + '=';

        Command::ptr c = parser.symbol(token, Command::ptr());
        if(c) str += c->texRepr(escape);
        else str += "undefined";
    }
    parser.logger()->log(Logger::SHOW, str, parser, parser.lastToken());
    return true;
}

bool ShowThe::parseArgs(Parser& parser, Node::ptr node)
{
    Node::ptr integer = parser.tryParseInternalInteger();
    if(integer) {
        node->appendChild("internal_quantity", integer);
        return true;
    }

    parser.logger()->log(Logger::ERROR,
        string("You can't use `") + parser.peekToken()->texRepr() +
        string("' after \\the"),
        parser, parser.lastToken()); //TODO: escapechar
    node->setValue(int(0));

    node->appendChild("internal_quantity", parser.parseToken());
    node->child("internal_quantity")->setValue(int(0));

    return true;
}

bool ShowThe::execute(Parser& parser, Node::ptr node)
{
    const any& value = node->child("internal_quantity")->valueAny();
    if(value.type() == typeid(int)) {
        parser.logger()->log(Logger::SHOW,
            boost::lexical_cast<string>(*unsafe_any_cast<int>(&value)),
            parser, parser.lastToken());
    }
    return true;
}

} // namespace base
} // namespace texpp

