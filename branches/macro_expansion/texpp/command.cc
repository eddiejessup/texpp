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

#include <texpp/command.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/foreach.hpp>

namespace texpp {

string Command::texRepr(Parser* parser) const
{
    if(!m_name.empty() && m_name[0] == '\\') {
        string ret = m_name;
        ret[0] = parser ? parser->symbol("escapechar", int(0)) : '\\';
        return ret;
    }
    return m_name;
}

string Command::repr() const
{
    return "Command(" + reprString(name())
            + ", " + reprString(texRepr()) + ")";
}

bool Command::checkPrefixes(Parser& parser)
{
    if(!parser.activePrefixes().empty()) {
        parser.logger()->log(Logger::ERROR,
            "You can't use a prefix with `" + texRepr(&parser) + "'",
            parser, parser.lastToken());
        parser.activePrefixes().clear();
    }
    return true;
}

bool Command::checkPrefixesGlobal(Parser& parser)
{
    bool ok = true;
    BOOST_FOREACH(const string& s, parser.activePrefixes()) {
        if(s != "\\global") ok = false;
    }

    if(!ok) {
        char escape = parser.symbol("escapechar", int('\\'));
        parser.logger()->log(Logger::ERROR,
            string("You can't use `") + escape + "long' or `" +
            escape + "outer' with `" + texRepr(&parser) + "'",
            parser, parser.lastToken());

        bool global = parser.isPrefixActive("\\global");
        parser.activePrefixes().clear();
        if(global) parser.activePrefixes().insert("\\global");
    }
    return true;
}

bool Command::checkPrefixesMacro(Parser& parser)
{
    bool ok = true;
    BOOST_FOREACH(const string& s, parser.activePrefixes()) {
        if(s != "\\global" && s != "\\outer" && s != "\\long")
            ok = false;
    }

    if(!ok) {
        parser.logger()->log(Logger::ERROR,
            "You can't use such a prefix with `" + texRepr(&parser) + "'",
            parser, parser.lastToken());

        bool global = parser.isPrefixActive("\\global");
        bool outer = parser.isPrefixActive("\\outer");
        bool long_ = parser.isPrefixActive("\\long");
        parser.activePrefixes().clear();
        if(global) parser.activePrefixes().insert("\\global");
        if(outer) parser.activePrefixes().insert("\\outer");
        if(long_) parser.activePrefixes().insert("\\long");
    }
    return true;
}

string TokenCommand::texRepr(Parser*) const
{
    return m_token->meaning();
}

bool TokenCommand::invoke(Parser&, shared_ptr<Node> node)
{
    node->setValue(m_token);
    return true;
}

Token::list Macro::stringToTokens(const string& str)
{
    Token::list ret;

    BOOST_FOREACH(char ch, str) {
        Token::CatCode catcode = Token::CC_OTHER;
        if(ch == ' ') catcode = Token::CC_SPACE;

        ret.push_back(Token::ptr(new Token(
            Token::TOK_CHARACTER, catcode, string(1, ch))));
    }

    return ret;
}

} // namespace texpp

