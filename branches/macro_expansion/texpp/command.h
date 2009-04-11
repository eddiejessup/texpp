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

#include <set>
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
    virtual string texRepr(Parser* parser = NULL) const;

    virtual bool invoke(Parser&, shared_ptr<Node>) { return true; }
    virtual bool invokeWithPrefixes(Parser&, shared_ptr<Node>,
                                std::set<string>&) { return false; }

protected:
    string m_name;
};

class TokenCommand: public Command
{
public:
    TokenCommand(shared_ptr<Token> token)
        : Command("token_command"), m_token(token) {}

    const shared_ptr<Token>& token() const { return m_token; }

    string texRepr(Parser* parser = NULL) const;
    bool invoke(Parser& parser, shared_ptr<Node> node);

protected:
    shared_ptr<Token> m_token;
};

class Macro: public Command
{
public:
    typedef shared_ptr<Macro> ptr;

    Macro(const string& name = string()): Command(name) {}

    bool invokeWithPrefixes(Parser&, shared_ptr<Node>,
                                std::set<string>&) { return true; }
    virtual bool expand(Parser&, shared_ptr<Node>) { return false; }

    static vector<shared_ptr<Token> > stringToTokens(const string& str);
};

class Conditional: public Macro
{
public:
    typedef shared_ptr<Conditional> ptr;
    Conditional(const string& name = string()): Macro(name) {}
};

class ConditionalBegin: public Conditional
{
public:
    typedef shared_ptr<ConditionalBegin> ptr;
    ConditionalBegin(const string& name = string()): Conditional(name) {}
    virtual bool evaluate(Parser&, shared_ptr<Node>) { return true; }
};

class ConditionalElse: public Conditional
{
public:
    typedef shared_ptr<ConditionalElse> ptr;
    ConditionalElse(const string& name = string()): Conditional(name) {}
};

class ConditionalEnd: public Conditional
{
public:
    typedef shared_ptr<ConditionalEnd> ptr;
    ConditionalEnd(const string& name = string()): Conditional(name) {}
};


} // namespace texpp

#endif


