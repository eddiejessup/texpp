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

#ifndef __TEXPP_BASE_BOX_H
#define __TEXPP_BASE_BOX_H

#include <texpp/common.h>
#include <texpp/command.h>
#include <texpp/token.h>
#include <texpp/parser.h>

#include <texpp/base/variable.h>

namespace texpp {
namespace base {

class Box: public Variable
{
public:
    Box(const string& name,
        const any& initValue = any(Token::list()))
        : Variable(name, initValue) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);
};

class Lastbox: public Box
{
public:
    Lastbox(const string& name,
        const any& initValue = any(Token::list()))
        : Box(name, initValue) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);
};

class Vsplit: public Register<Box>
{
public:
    Vsplit(const string& name,
        const any& initValue = any(Token::list()))
        : Register<Box>(name, initValue) {}

    string parseName(Parser& parser, shared_ptr<Node> node);
};

class Setbox: public Variable
{
public:
    Setbox(const string& name,
        const any& initValue = any(Token::list()))
        : Variable(name, initValue) {}

    string parseName(Parser& parser, shared_ptr<Node> node);
    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);
};

class BoxSpec: public Box
{
public:
    BoxSpec(const string& name, Parser::Mode mode)
        : Box(name, Token::list()), m_mode(mode) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);
protected:
    Parser::Mode m_mode;
};

} // namespace base
} // namespace texpp

#endif


