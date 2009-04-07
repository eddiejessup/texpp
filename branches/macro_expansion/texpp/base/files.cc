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


bool Message::invoke(Parser& parser, Node::ptr node)
{
    using boost::lexical_cast;
    Node::ptr text = parser.parseGeneralText();
    node->appendChild("text", text);
    
    string str;
    Token::list_ptr tokens =
        text->child("balanced_text")->value(Token::list_ptr());

    if(tokens) {
        Token::list tokens_show;
        BOOST_FOREACH(Token::ptr token, *tokens) {
            Command::ptr cmd = parser.symbol(token, Command::ptr());
            if(token->isControl() && !cmd) {
                parser.logger()->log(Logger::ERROR,
                    "Undefined control sequence", parser, token);
            } else {
                tokens_show.push_back(token);
            }
        }
        str = Token::texReprList(tokens_show, &parser);
    }
    parser.logger()->log(Logger::MESSAGE, str, parser, parser.lastToken());
                //text->child("right_brace")->value(Token::ptr()));
    return true;
}

} // namespace base
} // namespace texpp

