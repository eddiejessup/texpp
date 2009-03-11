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
#include <texpp/command.h>

#include <texpp/base/variable.h>

namespace texpp {
namespace base {

class InternalInteger: public Variable
{
public:
    InternalInteger(const string& name, const any& initValue = any(0))
        : Variable(name, initValue) {}
    bool parseArgs(Parser& parser, shared_ptr<Node> node);
    bool execute(Parser& parser, shared_ptr<Node> node);
};

class IntegerVariable: public InternalInteger
{
public:
    IntegerVariable(const string& name, const any& initValue = any(0))
        : InternalInteger(name, initValue) {}
};

class EndlinecharVariable: public IntegerVariable
{
public:
    EndlinecharVariable(const string& name, const any& initValue = any())
        : IntegerVariable(name, initValue) {}
    bool set(Parser& parser, const any& value, bool global = false);
};

class CharcodeVariable: public InternalInteger
{
public:
    CharcodeVariable(const string& name,
        const any& initValue = any(), int min=0, int max=0)
        : InternalInteger(name, initValue), m_min(min), m_max(max) {}

    bool check(Parser& parser, shared_ptr<Node> node);
    int min() const { return m_min; }
    int max() const { return m_max; }

protected:
    int m_min;
    int m_max;
};

class CatcodeVariable: public CharcodeVariable
{
public:
    CatcodeVariable(const string& name,
        const any& initValue = any(), int min=0, int max=0)
        : CharcodeVariable(name, initValue, min, max) {}
    bool set(Parser& parser, const any& value, bool global = false);
};

template<class Cmd, int MIN, int MAX>
class CharcodeVariableGroup: public FixedVariableGroup<Cmd>
{
public:
    CharcodeVariableGroup(const string& name,
                size_t maxCount, const any& initValue)
        : FixedVariableGroup<Cmd>(name, maxCount, initValue) {}

    Command::ptr createCommand(const string& name) {
        return Command::ptr(new Cmd(name, int(0), MIN, MAX));
    }
    string groupType() const { return "character"; }
};

} // namespace base
} // namespace texpp

#endif

