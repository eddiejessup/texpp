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

#include <texpp/base/files.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

namespace texpp {
namespace base {

bool Message::parseArgs(Parser& parser, Node::ptr node)
{
    parser.parseGeneralText(node);
    return true;
}

bool Message::execute(Parser& parser, Node::ptr node)
{
    using boost::lexical_cast;

    string str;
    Token::list_ptr tokens =
        node->child("balanced_text")->value(Token::list_ptr());

    if(tokens) {
        char newlinechar = parser.symbol("newlinechar", int(0));
        char escapechar = parser.symbol("escapechar", int(0));
        BOOST_FOREACH(Token::ptr token, *tokens) {
            Command::ptr cmd = parser.symbol(token, Command::ptr());
            if(cmd) {
                str += token->texRepr(escapechar);
                if(token->value().size() > 1 &&
                        token->value()[0] == '\\') {
                     int ccode = parser.symbol("catcode" +
                        lexical_cast<string>(int(token->value()[1])), int(0));
                    if(ccode == Token::CC_LETTER) str += ' ';
                }
            } else if(token->isControl()) {
                parser.logger()->log(Logger::ERROR,
                    "Undefined control sequence", parser, token);
            } else if(token->isCharacter(newlinechar)) {
                str += '\n';
            } else if(token->isCharacter()) {
                str += token->value();
            }
        }
    }
    parser.logger()->log(Logger::MESSAGE, str, parser, 
                node->child("right_brace")->value(Token::ptr()));
    return true;
}

} // namespace base
} // namespace texpp

