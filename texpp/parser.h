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

#include <deque>
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

class Node
{
public:
    typedef shared_ptr<Node> ptr;
    typedef vector< pair< string, Node::ptr > > ChildrenList;

    Node(const string& type): m_type(type) {}

    string source() const { return string(); }

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
    Parser(const string& fileName, std::istream* file,
            bool interactive = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>());

    Parser(const string& fileName, std::auto_ptr<std::istream> file,
            bool interactive = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>());

    Node::ptr parse();

    //////// Tokens
    Token::ptr lastToken();
    Token::ptr peekToken();
    Token::ptr nextToken(vector< Token::ptr >* tokens = NULL);
    void end() { m_end = true; }

    //////// Parse helpers
    bool helperIsImplicitCharacter(Token::CatCode catCode);

    bool parseCommandArgs(Command::ptr command, Node::ptr node) {
        return command->parseArgs(*this, node);
    }

    bool executeCommand(Command::ptr command, Node::ptr node) {
        return command->execute(*this, node);
    }

    Node::ptr parseGroup(Command::ptr endCmd = Command::ptr(),
                         bool parseBeginEnd = true);

    Node::ptr parseCommand(Command::ptr command);

    Node::ptr parseToken();
    Node::ptr parseControlSequence();
    Node::ptr parseCharacter();

    Node::ptr parseOptionalEquals(bool oneSpaceAfter);
    Node::ptr parseOptionalSigns();
    Node::ptr tryParseInternalInteger();
    Node::ptr parseNormalInteger();
    Node::ptr parseNumber();

    Node::ptr parseBalancedText();
    Node::ptr parseGeneralText(
            Node::ptr node = Node::ptr());

    Node::ptr parseTextWord();

    template<class Cmd>
    shared_ptr<Cmd> parseCommandOrGroup(Node::ptr node);

    //////// Symbols
    void setSymbol(const string& name, const any& value, bool global = false);
    void setSymbol(Token::ptr token, const any& value, bool global = false) {
        if(token && token->isControl())
            setSymbol(token->value(), value, global);
    }
    
    const any& symbolAny(const string& name, bool global = false) const;
    const any& symbolAny(Token::ptr token, bool global = false) const {
        if(!token || !token->isControl()) return EMPTY_ANY;
        else return symbolAny(token->value(), global);
    }
    
    template<typename T>
    T symbol(const string& name, T def, bool global = false) {
        const any& v = symbolAny(name, global);
        if(v.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&v);
    }

    template<typename T>
    T symbol(Token::ptr token, T def, bool global = false) {
        const any& v = symbolAny(token, global);
        if(v.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&v);
    }

    template<typename T>
    shared_ptr<T> symbolCommand(Token::ptr token, bool global = false) {
        return dynamic_pointer_cast<T>(
                symbol(token, Command::ptr(), global));
    }

    void beginGroup();
    void endGroup();

    //////// Others
    shared_ptr<Logger> logger() { return m_logger; }
    shared_ptr<Lexer> lexer() { return m_lexer; }

protected:
    Token::ptr rawNextToken();

    typedef std::deque<
        Token::ptr
    > TokenQueue;

    shared_ptr<Lexer>   m_lexer;
    shared_ptr<Logger>  m_logger;

    Token::ptr      m_token;
    Token::ptr      m_lastToken;
    TokenQueue      m_tokenQueue;

    int             m_groupLevel;
    bool            m_end;

    typedef unordered_map<
        string, pair< int, any >
    > SymbolTable;

    typedef vector<
        pair<string, pair<int, any> >
    > SymbolStack;

    SymbolTable     m_symbols;
    SymbolTable     m_symbolsGlobal;
    SymbolStack     m_symbolsStack;
    vector<size_t>  m_symbolsStackLevels;

    static any EMPTY_ANY;
};

template<class Cmd>
shared_ptr<Cmd> Parser::parseCommandOrGroup(Node::ptr node)
{
    Command::ptr cmd = symbol(peekToken(), Command::ptr());
    if(dynamic_pointer_cast<Cmd>(cmd)) {
        node->appendChild("command", parseControlSequence());
        return static_pointer_cast<Cmd>(cmd);
    }
    shared_ptr<CommandGroupBase> gr =
        dynamic_pointer_cast<CommandGroupBase>(cmd);
    if(gr && dynamic_pointer_cast<Cmd>(gr->item(0))) {
        node->appendChild("command", parseControlSequence());
        return static_pointer_cast<Cmd>(gr->parseCommand(*this, node));
    }
    return shared_ptr<Cmd>();
}

} // namespace texpp

#endif

