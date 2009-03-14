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

#include <texpp/base/toks.h>
#include <texpp/parser.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool InternalToks::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("equals", parser.parseOptionalEquals(false));
    node->appendChild("rvalue", parser.parseGeneralText());
    return check(parser, node->child("rvalue"));
}

bool InternalToks::execute(Parser& parser, Node::ptr node)
{
    Token::list_ptr tokens = node->child("rvalue")
            ->child("balanced_text")->value(Token::list_ptr());
    return set(parser, tokens ? *tokens : Token::list());
}

string InternalToks::toksToString(Parser& parser, const Token::list& toks)
{
    string str;
    char newlinechar = parser.symbol("newlinechar", int(0));
    char escapechar = parser.symbol("escapechar", int(0));
    BOOST_FOREACH(Token::ptr token, toks) {
        if(token->isControl()) {
            str += token->texRepr(escapechar);
            if(token->value().size() > 1 &&
                    token->value()[0] == '\\') {
                 int ccode = parser.symbol("catcode" +
                    boost::lexical_cast<string>(int(token->value()[1])),
                    int(0));
                if(ccode == Token::CC_LETTER) str += ' ';
            }
        } else if(token->isCharacter(newlinechar)) {
            str += '\n';
        } else if(token->isCharacter()) {
            str += token->value();
        }
    }
    return str;
}

} // namespace base
} // namespace texpp

