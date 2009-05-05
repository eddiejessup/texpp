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

#ifndef __TEXPP_PARSER_H
#define __TEXPP_PARSER_H

#include <texpp/common.h>
#include <texpp/lexer.h>
#include <texpp/command.h>
#include <texpp/command.h>

#include <deque>
#include <set>
#include <cassert>
#include <climits>

#include <boost/any.hpp>

namespace texpp {

using boost::any;
using boost::any_cast;
using boost::unsafe_any_cast;

class Lexer;
class Logger;
class Parser;

namespace base {
    class ExpandafterMacro;
} // namespace base

class Node
{
public:
    typedef shared_ptr<Node> ptr;
    typedef vector< pair< string, Node::ptr > > ChildrenList;

    Node(const string& type): m_type(type) {}

    string source() const;

    const string& type() const { return m_type; }
    void setType(const string& type) { m_type = type; }

    void setValue(const any& value) { m_value = value; }
    const any& valueAny() const { return m_value; }
    template<typename T> T value(T def) const {
        if(m_value.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&m_value);
    }

    const vector< Token::ptr >& tokens() const { return m_tokens; }
    vector< Token::ptr >& tokens() { return m_tokens; }

    const ChildrenList& children() const { return m_children; }
    ChildrenList& children() { return m_children; }

    size_t childrenCount() const { return m_children.size(); }
    Node::ptr child(int num) { return m_children[num].second; }
    Node::ptr child(const string& name);

    void appendChild(const string& name, Node::ptr node) {
        m_children.push_back(make_pair(name, node));
    }

    Token::ptr lastToken();

    string repr() const;
    string treeRepr(size_t indent = 0) const;

protected:
    string                  m_type;
    any                     m_value;
    vector< Token::ptr >    m_tokens;

    ChildrenList            m_children;
};

class Parser
{
public:
    enum Interaction { ERRORSTOPMODE, SCROLLMODE,
                       NONSTOPMODE, BATCHMODE };
    enum Mode { NULLMODE,
                VERTICAL, HORIZONTAL,
                RVERTICAL, RHORIZONTAL,
                MATH, DMATH };
    enum GroupType { GROUP_DOCUMENT, GROUP_NORMAL,
                     GROUP_MATH, GROUP_DMATH,
                     GROUP_CUSTOM };

    Parser(const string& fileName, std::istream* file,
            bool interactive = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>());

    Parser(const string& fileName, shared_ptr<std::istream> file,
            bool interactive = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>());

    Interaction interaction() const { return m_interaction; }
    void setInteraction(Interaction intr) { m_interaction = intr; }

    Node::ptr parse();

    const string& modeName() const;
    Mode mode() const { return m_mode; }
    void setMode(Mode mode) { m_mode = mode; }

    bool hasOutput() const { return m_hasOutput; }

    void traceCommand(Token::ptr token, bool expanding = false);

    //////// Tokens
    Token::ptr lastToken();
    Token::ptr peekToken(bool expand = true);
    Token::ptr nextToken(vector< Token::ptr >* tokens = NULL,
                         bool expand = true);
    void setNoexpand(Token::ptr token) { m_noexpandToken = token; }
    void resetNoexpand() { m_noexpandToken.reset(); pushBack(NULL); }
    void pushBack(vector< Token::ptr >* tokens);

    void end() { m_end = true; }

    //////// Parse helpers
    bool helperIsImplicitCharacter(Token::CatCode catCode,
                                        bool expand = true);

    Node::ptr parseGroup(GroupType groupType,
                                bool parseBeginEnd = true);

    Node::ptr parseCommand(Command::ptr command);

    Node::ptr parseToken(bool expand = true);
    Node::ptr parseDMathToken();
    Node::ptr parseControlSequence(bool expand = true);

    Node::ptr parseOptionalSpaces();

    Node::ptr parseKeyword(const vector<string>& keywords);
    Node::ptr parseOptionalKeyword(const vector<string>& keywords);

    Node::ptr parseOptionalEquals();
    Node::ptr parseOptionalSigns();
    Node::ptr parseNormalInteger();
    Node::ptr parseNumber();
    Node::ptr parseDimenFactor();
    Node::ptr parseNormalDimen(bool fil = false, bool mu = false);
    Node::ptr parseDimen(bool fil = false, bool mu = false);
    Node::ptr parseGlue(bool mu = false);

    Node::ptr parseFiller(bool expand);
    Node::ptr parseBalancedText(bool expand, int paramCount = -1,
                                    Token::ptr nameToken = Token::ptr());
    Node::ptr parseGeneralText(bool expand, bool implicitLbrace = true);

    Node::ptr parseFileName();

    Node::ptr parseTextWord();
    Node::ptr parseTextCharacter();

    void processTextCharacter(char ch, Token::ptr token);
    void resetParagraphIndent();

    //////// Symbols
    void setSymbol(const string& name, const any& value, bool global = false);
    void setSymbol(Token::ptr token, const any& value, bool global = false) {
        if(token && token->isControl())
            setSymbol(token->value(), value, global);
    }

    void setSymbolDefault(const string& name, const any& defaultValue);
    
    const any& symbolAny(const string& name) const;
    const any& symbolAny(Token::ptr token) const {
        if(!token || !token->isControl()) return EMPTY_ANY;
        else return symbolAny(token->value());
    }
    
    template<typename T>
    T symbol(const string& name, T def) const {
        const any& v = symbolAny(name);
        if(v.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&v);
    }

    template<typename T>
    T symbol(Token::ptr token, T def) const {
        const any& v = symbolAny(token);
        if(v.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&v);
    }

    template<typename T>
    shared_ptr<T> symbolCommand(Token::ptr token) const {
        return dynamic_pointer_cast<T>(
                symbol(token, Command::ptr()));
    }

    void beginGroup();
    void endGroup();

    void beginCustomGroup(const string& type) {
        m_customGroupBegin = true; m_customGroupType = type; beginGroup(); }
    void endCustomGroup() { endGroup(); m_customGroupEnd = true; }

    string escapestr() const {
        int e = symbol("escapechar", int(0));
        return e >= 0 && e <= 255 ? string(1, e) : string();
    }

    //////// Others
    shared_ptr<Logger> logger() { return m_logger; }
    shared_ptr<Lexer> lexer() { return m_lexer; }

    static const string& banner() { return BANNER; }

protected:
    Node::ptr rawExpandToken(Token::ptr token);
    Token::ptr rawNextToken(bool expand = true);
    Node::ptr parseFalseConditional(size_t level,
                          bool sElse = false, bool sOr = false);
    void setSpecialSymbol(const string& name, const any& value);
    void init();

    typedef std::deque<
        Token::ptr
    > TokenQueue;

    shared_ptr<Lexer>   m_lexer;
    shared_ptr<Logger>  m_logger;

    Token::ptr      m_token;
    Token::list     m_tokenSource;

    Token::ptr      m_lastToken;
    Token::ptr      m_noexpandToken;
    TokenQueue      m_tokenQueue;

    int             m_groupLevel;
    bool            m_end;

    struct ConditionalInfo {
        bool parsed;
        bool ifcase;
        bool active;
        int  value;
        int  branch;
    };
    vector< ConditionalInfo >
                    m_conditionals;

    typedef unordered_map<
        string, pair< int, any >
    > SymbolTable;

    typedef vector<
        pair<string, pair<int, any> >
    > SymbolStack;

    SymbolTable     m_symbols;
    SymbolStack     m_symbolsStack;
    vector<size_t>  m_symbolsStackLevels;

    size_t          m_lineNo;
    Mode            m_mode;
    Mode            m_prevMode;

    bool            m_hasOutput;

    GroupType       m_currentGroupType;

    string  m_customGroupType;
    bool    m_customGroupBegin;
    bool    m_customGroupEnd;

    Interaction m_interaction;

    static any EMPTY_ANY;
    static string BANNER;

    friend class base::ExpandafterMacro;
};

} // namespace texpp

#endif
