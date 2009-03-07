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

#include <texpp/base/files.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {
namespace base {

Node::ptr Message::parse(Parser& parser)
{
    Node::ptr node(new Node(name()));
    node->appendChild("command", parser.parseControlSequence());
    parser.parseGeneralText(node);
    return node;
}

bool Message::execute(Parser& parser, Node::ptr node)
{
    /*
    string str;
    Token::list_ptr tokens =
        node->child("balanced_text")->value(Token::list_ptr());

    if(tokens) {
        for(size_t i=0; i<tokens->size(); ++i) {
            if((*tokens)[i]->isControl()) {
                str += (*tokens)[i]->controlRepr(
                                parser.symbol("escapechar", int(0)));
                if((*tokens)[i]->value.size() > 1 &&
                        (*tokens)[i]->value[0] == '\\' &&
                        parser.symbol("catcode`"+string(1,
                            (*tokens)[i]->value[1])) == Token::CC_LETTER) {
                    str += ' ';
                }
            } else if(tokens[i]->isCharacter()) {
                str += tokens[i]->value;
            }
        }
    }
    parser.logger()->log(Logger::MESSAGE, str,
                node->child("right_brace")->value<TokenObject>()->token());
    */
    return true;
}

} // namespace base
} // namespace texpp

