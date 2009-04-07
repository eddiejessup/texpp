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

#ifndef __TEXPP_BASE_FUNC_H
#define __TEXPP_BASE_FUNC_H

#include <texpp/common.h>
#include <texpp/command.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <iostream>

namespace texpp {
namespace base {

class Prefix: public Command
{
public:
    explicit Prefix(const string& name): Command(name) {}
    bool invoke(Parser& parser, shared_ptr<Node> node);
    bool checkPrefixes(Parser&) { return false; }
};

class Let: public Command
{
public:
    explicit Let(const string& name): Command(name) {}
    bool invoke(Parser& parser, shared_ptr<Node> node);
    bool checkPrefixes(Parser& parser) {
        return checkPrefixesGlobal(parser);
    }
};

class Futurelet: public Command
{
public:
    explicit Futurelet(const string& name): Command(name) {}
    bool invoke(Parser& parser, shared_ptr<Node> node);
    bool checkPrefixes(Parser& parser) {
        return checkPrefixesGlobal(parser);
    }
};

template<class Cmd>
class RegisterDef: public Command
{
public:
    RegisterDef(const string& name, shared_ptr<Cmd> group)
        : Command(name), m_group(group) {}

    shared_ptr<Cmd> group() { return m_group; }

    bool invoke(Parser& parser, shared_ptr<Node> node);
    bool checkPrefixes(Parser& parser) {
        return checkPrefixesGlobal(parser);
    }

protected:
    shared_ptr<Cmd> m_group;
};

template<class Cmd>
bool RegisterDef<Cmd>::invoke(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr lvalue = parser.parseControlSequence();
    node->appendChild("lvalue", lvalue);
    node->appendChild("equals", parser.parseOptionalEquals());

    Node::ptr rvalue = parser.parseNumber();
    node->appendChild("rvalue", rvalue);

    Token::ptr ltoken = lvalue->value(Token::ptr());
    int num = rvalue->value(int(0));

    return m_group->createDef(parser, ltoken, num);
}

class Def: public Command
{
public:
    explicit Def(const string& name): Command(name) {}
    bool invoke(Parser& parser, shared_ptr<Node> node);
    bool checkPrefixes(Parser& parser) {
        return checkPrefixesMacro(parser);
    }
};

class UserMacro: public Macro
{
public:
    explicit UserMacro(const string& name,
        Token::list_ptr params, Token::list_ptr definition)
        : Macro(name), m_params(params), m_definition(definition) {}

    Token::list params() { return *m_params; }
    Token::list definition() { return *m_definition; }

    string texRepr(Parser* parser = NULL) const;

protected:
    Token::list_ptr m_params;
    Token::list_ptr m_definition;
};

} // namespace base
} // namespace texpp


#endif

