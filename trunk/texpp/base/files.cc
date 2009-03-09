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

namespace texpp {
namespace base {

bool Message::parseArgs(Parser& parser, Node::ptr node)
{
    parser.parseGeneralText(node);
    return true;
}

bool Message::execute(Parser& parser, Node::ptr node)
{
    string str;
    Token::list_ptr tokens =
        node->child("balanced_text")->value(Token::list_ptr());

    if(tokens) {
        for(size_t i=0; i<tokens->size(); ++i) {
            if((*tokens)[i]->isControl()) {
                str += (*tokens)[i]->texRepr(
                                parser.symbol("escapechar", int(0)));
                if((*tokens)[i]->value().size() > 1 &&
                        (*tokens)[i]->value()[0] == '\\' &&
                        parser.symbol("catcode`" + 
                            (*tokens)[i]->value()[1], int(0)) ==
                            Token::CC_LETTER) {
                    str += ' ';
                }
            } else if((*tokens)[i]->isCharacter()) {
                str += (*tokens)[i]->value();
            }
        }
    }
    parser.logger()->log(Logger::MESSAGE, str, parser, 
                node->child("right_brace")->value(Token::ptr()));
    return true;
}

} // namespace base
} // namespace texpp

