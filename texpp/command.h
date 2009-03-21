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

#ifndef __TEXPP_COMMAND_H
#define __TEXPP_COMMAND_H

#include <texpp/common.h>

#include <boost/lexical_cast.hpp>

namespace texpp {

class Token;
class Node;
class Parser;

class Command
{
public:
    typedef shared_ptr<Command> ptr;

    Command(const string& name = string()): m_name(name) {}
    virtual ~Command() {}

    const string& name() const { return m_name; }

    virtual string repr() const;
    virtual string texRepr(char escape = '\\') const;

    virtual bool parseArgs(Parser&, shared_ptr<Node>) { return false; }
    virtual bool execute(Parser&, shared_ptr<Node>) { return false; }

protected:
    string m_name;
};

class TokenCommand: public Command
{
public:
    TokenCommand(shared_ptr<Token> token)
        : Command("token_command"), m_token(token) {}

    const shared_ptr<Token>& token() const { return m_token; }

    string texRepr(char escape = '\\') const;
    bool parseArgs(Parser&, shared_ptr<Node>);
    bool execute(Parser&, shared_ptr<Node>);

protected:
    shared_ptr<Token> m_token;
};

class CommandGroupBase: public Command
{
public:
    CommandGroupBase(const string& name) : Command(name) {}

    virtual Command::ptr item(size_t) { return Command::ptr(); }
    virtual Command::ptr createCommand(const string&) {
        return Command::ptr();
    }

    virtual string groupType() const { return "group"; }

    virtual Command::ptr parseCommand(Parser& parser, shared_ptr<Node> node);

    bool parseArgs(Parser& parser, shared_ptr<Node> node);
    bool execute(Parser& parser, shared_ptr<Node> node);
};

template<class Cmd>
class FixedCommandGroup: public CommandGroupBase
{
public:
    FixedCommandGroup(const string& name, size_t maxCount)
        : CommandGroupBase(name), m_maxCount(maxCount) {}

    Command::ptr item(size_t n);
    Command::ptr createCommand(const string& name) {
        return Command::ptr(new Cmd(name));
    }

protected:
    size_t m_maxCount;
    vector<Command::ptr> m_items;
};

template<class Cmd>
Command::ptr FixedCommandGroup<Cmd>::item(size_t n)
{
    if(n >= m_maxCount) return Command::ptr();
    if(n >= m_items.size()) m_items.resize(n+1);
    if(!m_items[n]) {
        m_items[n] = createCommand(
            name() + boost::lexical_cast<string>(n));
    }
    return m_items[n];
}

} // namespace texpp

#endif


