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

#ifndef __TEXPP_TOKEN_H
#define __TEXPP_TOKEN_H

#include <texpp/common.h>

namespace texpp {

class Parser;

class Token
{
public:
    typedef shared_ptr<Token> ptr;
    typedef vector<Token::ptr> list;
    typedef shared_ptr<list> list_ptr;

    enum Type {
        TOK_SKIPPED,
        TOK_CHARACTER,
        TOK_CONTROL
    };

    enum CatCode {
        CC_ESCAPE = 0,
        CC_BGROUP = 1,
        CC_EGROUP = 2,
        CC_MATHSHIFT = 3,
        CC_ALIGNTAB = 4,
        CC_EOL = 5,
        CC_PARAM = 6,
        CC_SUPER = 7,
        CC_SUB = 8,
        CC_IGNORED = 9,
        CC_SPACE = 10,
        CC_LETTER = 11,
        CC_OTHER = 12,
        CC_ACTIVE = 13,
        CC_COMMENT = 14,
        CC_INVALID = 15,
        CC_NONE = 16
    };

    Token(Type type = TOK_SKIPPED, CatCode catCode = CC_INVALID,
            const string& value = string(), const string& source = string(),
            size_t lineNo = 0, size_t charPos = 0, size_t charEnd = 0)
        : m_type(type), m_catCode(catCode), m_value(value), m_source(source),
          m_lineNo(lineNo), m_charPos(charPos), m_charEnd(charEnd) {}

    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }

    CatCode catCode() const { return m_catCode; }
    void setCatCode(CatCode catCode) { m_catCode = catCode; }

    const string& value() const { return m_value; }
    void setValue(const string& value) { m_value = value; }

    const string& source() const { return m_source; }
    void setSource(const string& source) { m_source = source; }

    size_t lineNo() const { return m_lineNo; }
    void setLineNo(size_t lineNo) { m_lineNo = lineNo; }

    size_t charPos() const { return m_charPos; }
    void setCharPos(size_t charPos) { m_charPos = charPos; }

    size_t charEnd() const { return m_charEnd; }
    void setCharEnd(size_t charEnd) { m_charEnd = charEnd; }

    bool isSkipped() const { return m_type == TOK_SKIPPED; }
    bool isControl() const { return m_type == TOK_CONTROL; }
    bool isCharacter() const { return m_type == TOK_CHARACTER; }

    bool isCharacter(char c) const {
        return m_type == TOK_CHARACTER && m_value[0] == c;
    }

    bool isCharacter(char c, CatCode cat) const {
        return m_type == TOK_CHARACTER && m_value[0] == c && m_catCode == cat;
    }

    bool isCharacterCat(CatCode cat) {
        return m_type == TOK_CHARACTER && m_catCode == cat;
    }

    bool isLastInLine() const {
        char last = m_source.empty() ? 0 : m_source[m_source.size()-1];
        return last == '\r' || last == '\n';
    }

    string texRepr(Parser* parser = NULL) const;
    string meaning(Parser* parser = NULL) const;
    string repr() const;

    Token::ptr lcopy() const {
        return Token::ptr(new Token(
            m_type, m_catCode, m_value, "", m_lineNo, m_charEnd, m_charEnd));
    }

    static string texReprControl(const string& name,
                                Parser* parser = NULL, bool space = false);
    static string texReprList(const Token::list& tokens,
                                Parser* parser = NULL);

protected:
    Type        m_type;
    CatCode     m_catCode;
    string      m_value;
    string      m_source;

    size_t      m_lineNo;
    size_t      m_charPos;
    size_t      m_charEnd;
};

} // namespace texpp

#endif
