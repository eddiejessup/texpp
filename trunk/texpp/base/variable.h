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

#ifndef __TEXPP_BASE_VARIABLES_H
#define __TEXPP_BASE_VARIABLES_H

#include <texpp/common.h>
#include <texpp/command.h>

namespace texpp {
namespace base {

class Variable: public Command
{
public:
    enum Operation { GET, ASSIGN, ADVANCE, MULTIPLY, DIVIDE, EXPAND };

    Variable(const string& name, const any& initValue = any())
        : Command(name), m_initValue(initValue) {}

    const any& initValue() const { return m_initValue; }

    virtual string parseName(Parser& parser, shared_ptr<Node> node);
    virtual bool invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op);

    bool invoke(Parser& parser, shared_ptr<Node> node);

protected:
    any m_initValue;
};

class ArithmeticCommand: public Command
{
public:
    explicit ArithmeticCommand(const string& name, Variable::Operation op)
        : Command(name), m_op(op) {}

    Variable::Operation operation() const { return m_op; }

    bool invoke(Parser& parser, shared_ptr<Node> node);

protected:
    Variable::Operation m_op;
};

} // namespace base
} // namespace texpp

#endif

