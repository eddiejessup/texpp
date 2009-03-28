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

#include <texpp/base/miscmacros.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool NumberMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("number", number);

    node->setValue(stringToTokens(
        boost::lexical_cast<string>(number->value(int(0)))));

    return true;
}

bool Romannumeral::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("number", number);

    static int BASIC_VALUES[] = { 1000, 900, 500, 400, 100, 90,
             50, 40, 10, 9, 5, 4, 1 };
    static const char* BASIC_ROMAN_NUMBERS[] = { "m", "cm", "d", "cd",
             "c", "xc", "l", "xl", "x", "ix", "v", "iv", "i" };

    string str;
    int num = number->value(int(0));
    for(size_t i = 0; i < sizeof(BASIC_VALUES)/sizeof(int); ++i) {
        while(num >= BASIC_VALUES[i]) {
            str += BASIC_ROMAN_NUMBERS[i];
            num -= BASIC_VALUES[i];
        }
    }

    node->setValue(stringToTokens(str));

    return true;
}

} // namespace base
} // namespace texpp


