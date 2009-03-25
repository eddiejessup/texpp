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
#include <texpp/base/integer.h>
#include <texpp/base/dimen.h>
#include <texpp/base/glue.h>
#include <texpp/base/toks.h>
#include <texpp/base/font.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool Show::invoke(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr tokenNode = parser.parseToken();
    Token::ptr token = tokenNode->value(Token::ptr());
    node->appendChild("token", tokenNode);

    string str;
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

bool ShowThe::invoke(Parser& parser, Node::ptr node)
{
    Node::ptr tokenNode = parser.parseToken();
    Token::ptr token = tokenNode->value(Token::ptr());
    node->appendChild("token", tokenNode);

    string str;
    shared_ptr<Variable> var = parser.symbolCommand<Variable>(token);
    if(var) {
        bool ok = var->invokeOperation(parser, node, Variable::EXPAND);
        if(ok) str = node->value(string());
    } else {
        parser.logger()->log(Logger::ERROR,
            "You can't use `" + token->texRepr() +
            "' after " + char(parser.symbol("escapechar", int(0))) + "the",
            parser, token);
        str = "0";
    }

    parser.logger()->log(Logger::SHOW, str,
        parser, parser.lastToken());

    return bool(var);
}

} // namespace base
} // namespace texpp

