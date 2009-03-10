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

#ifndef __TEXPP_BASE_INTEGER_H
#define __TEXPP_BASE_INTEGER_H

#include <texpp/common.h>
#include <texpp/parser.h>

#include <texpp/base/variable.h>

namespace texpp {
namespace base {

class IntegerVariable: public Variable
{
public:
    IntegerVariable(const string& name, const any& initValue = any(0))
        : Variable(name, initValue) {}
    bool parseArgs(Parser& parser, Node::ptr node);
    bool execute(Parser& parser, Node::ptr node);
};

class EndlinecharVariable: public IntegerVariable
{
public:
    EndlinecharVariable(const string& name, const any& initValue = any())
        : IntegerVariable(name, initValue) {}
    bool set(Parser& parser, const any& value, bool global = false);
};

class CharcodeVariable: public IntegerVariable
{
public:
    CharcodeVariable(const string& name, const any& initValue = any())
        : IntegerVariable(name, initValue) {}
    bool check(Parser& parser, Node::ptr node);
};

class CatcodeVariable: public CharcodeVariable
{
public:
    CatcodeVariable(const string& name, const any& initValue = any())
        : CharcodeVariable(name, initValue) {}
    bool set(Parser& parser, const any& value, bool global = false);
    bool check(Parser& parser, Node::ptr node);
};

template<class Cmd>
class CharcodeVariableGroup: public FixedVariableGroup<Cmd>
{
public:
    CharcodeVariableGroup(const string& name,
                size_t maxCount, const any& initValue)
        : FixedVariableGroup<Cmd>(name, maxCount, initValue) {}

    string groupType() const { return "character"; }
};

} // namespace base
} // namespace texpp

#endif

