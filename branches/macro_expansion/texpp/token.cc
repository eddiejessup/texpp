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

#include <texpp/token.h>

#include <sstream>
#include <iomanip>

namespace {
const texpp::string catCodeLongNames[] = {
    "escape character",
    "begin-group character",
    "end-group character",
    "math shift character",
    "alignment tab character",
    "end of line character",
    "macro parameter character",
    "superscript character",
    "subscript character",
    "ignored character",
    "space character",
    "the letter",
    "the character",
    "active character",
    "comment character",
    "invalid character",
};
const texpp::string typeNames[] = {
    "TOK_SKIPPED",
    "TOK_CHARACTER",
    "TOK_CONTROL"
};
const texpp::string catCodeNames[] = {
    "CC_ESCAPE",
    "CC_BGROUP",
    "CC_EGROUP",
    "CC_MATHSHIFT",
    "CC_ALIGNTAB",
    "CC_EOL",
    "CC_PARAM",
    "CC_SUPER",
    "CC_SUB",
    "CC_IGNORED",
    "CC_SPACE",
    "CC_LETTER",
    "CC_OTHER",
    "CC_ACTIVE",
    "CC_COMMENT",
    "CC_INVALID",
    "CC_NONE",
};
} // namespace

namespace texpp {

string Token::texRepr(char escape) const
{
    if(isControl()) {
        if(m_value[0] == '\\') {
            if(m_value.size() == 1) {
                return string(1, escape) + "csname"
                         + string(1, escape) + "endcsname";
            } else {
                return string(1, escape) + m_value.substr(1);
            }
        } else {
            return m_value.substr(1);
        }
    } else if(isCharacter()) {
        return m_value;
    } else {
        return string();
    }
}

string Token::meaning() const
{
    if(isCharacter()) {
        return catCodeLongNames[m_catCode] + " " + m_value;
    } else if(isControl()) {
        return "control sequence " + texRepr('\\');
    } else if(isSkipped()) {
        return "skipped characters";
    }
    return "";
}

string Token::repr() const
{
    std::ostringstream r;
    r << "Token(Token::" << (m_type < 3 ? typeNames[m_type] : "")
      << ", Token::" << (m_catCode < 16 ? catCodeNames[m_catCode] : "")
      << ", " << reprString(m_value)
      << ", " << reprString(m_source)
      << ", " << m_lineNo << ", " << m_charPos << ", " << m_charEnd << ")";
      //<< ", \"" << reprString(source()) << "\")";

    return r.str();
}

} // namespace texpp

