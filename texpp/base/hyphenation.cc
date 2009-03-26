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

#include <texpp/base/hyphenation.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {
namespace base {

bool Hyphenation::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN) {
        string name = parseName(parser, node);

        Node::ptr internal = parser.parseGeneralText();
        Token::list_ptr tokens = internal->child("balanced_text")
                                    ->value(Token::list_ptr());
        node->setValue(tokens ? *tokens : Token::list());

        node->appendChild("rvalue", internal);

        if(tokens) {
            Token::list h = parser.symbol(name, Token::list());
            std::copy(tokens->begin(), tokens->end(), std::back_inserter(h));
            parser.setSymbol(name, h);
        }
        return true;
    }
    return false;
}

} // namespace base
} // namespace texpp



