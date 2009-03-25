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

#include <texpp/base/variable.h>
#include <texpp/parser.h>
#include <texpp/logger.h>
#include <texpp/lexer.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

string Variable::reprValue(Parser&, shared_ptr<Node> node)
{
    return reprAny(node->valueAny());
}

string Variable::parseName(Parser&, shared_ptr<Node>)
{
    return name().substr(1);
}

bool Variable::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == GET) {
        string name = parseName(parser, node);
        const any& ret = parser.symbolAny(name);
        node->setValue(ret.empty() ? m_initValue : ret);
        return true;
    }
    return false;
}

bool Variable::invoke(Parser& parser, shared_ptr<Node> node)
{
    return invokeOperation(parser, node, ASSIGN);
}

const any& Variable::getAny(Parser& parser, bool global)
{
    const any& ret = parser.symbolAny(name().substr(1), global);
    return !ret.empty() ? ret : m_initValue;
}

bool Variable::set(Parser& parser, const any& value, bool global)
{
    string varname = name().substr(1);
    parser.setSymbol(varname, value, global);
    return true;
    /*
    any s = parser.symbolAny(varname, global);
    if(s.type() == value.type()) {
        parser.setSymbol(varname, value, global);
        return true;
    } else {
        return false;
    }*/
}

bool ArithmeticCommand::invoke(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr lvalue = parser.parseToken();
    Token::ptr token = lvalue->value(Token::ptr());

    bool ok = false;
    shared_ptr<Variable> var = parser.symbolCommand<Variable>(token);
    if(var) {
        node->appendChild("lvalue", lvalue);
        ok = var->invokeOperation(parser, node, m_op);
        if(!ok) node->children().pop_back();
    }

    if(!ok) {
        parser.logger()->log(Logger::ERROR,
            string("You can't use `") + token->texRepr() +
            string("' after ") + texRepr(),
            parser, token);
        node->setValue(int(0));
        node->appendChild("error_wrong_lvalue", lvalue);
    }

    return ok;
}

bool RegisterGroupDef::parseArgs(Parser& parser, shared_ptr<Node> node)
{
    node->appendChild("lvalue", parser.parseControlSequence());
    node->appendChild("equals", parser.parseOptionalEquals(false));
    node->appendChild("rvalue", parser.parseNumber());
    return true;
}

bool RegisterGroupDef::execute(Parser& parser, shared_ptr<Node> node)
{
    int item = node->child("rvalue")->value(int(0));
    Command::ptr cmd = m_group->item(item);
    if(!cmd) {
        parser.logger()->log(Logger::ERROR,
            "Bad register code (" + boost::lexical_cast<string>(item) + ")",
            parser, parser.lastToken());
        cmd = m_group->item(0);
    }
    parser.setSymbol(
        node->child("lvalue")->value(Token::ptr()),
        cmd);
    return true;
}

} // namespace base
} // namespace texpp

