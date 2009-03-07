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

#include <texpp/base/show.h>
#include <texpp/logger.h>

#include <sstream>

namespace texpp {
namespace base {

Node::ptr Show::parse(Parser& parser)
{
    Node::ptr node(new Node(name()));
    node->appendChild("command", parser.parseControlSequence());
    node->appendChild("token", parser.parseToken());
    return node;
}

bool Show::execute(Parser& parser, Node::ptr node)
{
    string str;
    Token::ptr token = node->child(1)->value(Token::ptr());
    if(token->isCharacter()) {
        str = token->meaning();
    } else {
        char escape = parser.symbol("escapechar", char(0));
        str = token->texRepr(escape) + '=';

        Command::ptr c = parser.symbol(token, Command::ptr());
        if(c) str += c->texRepr();
        else str += "undefined";
    }
    parser.logger()->log(Logger::SHOW, str, parser, token);
    return true;
}

} // namespace base
} // namespace texpp

