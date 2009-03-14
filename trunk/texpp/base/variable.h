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
    Variable(const string& name, const any& initValue = any())
        : Command(name), m_initValue(initValue) {}

    const any& initValue() const { return m_initValue; }
    void setInitValue(const any& initValue) { m_initValue = initValue; }

    virtual bool check(Parser&, shared_ptr<Node>) { return true; }
    virtual bool set(Parser& parser, const any& value, bool global = false);

    virtual const any& getAny(Parser& parser, bool global = false);

    template<typename T>
    T get(Parser& parser, T def, bool global = false) {
        const any& value = getAny(parser, global);
        if(value.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&value);
    }

protected:
    any m_initValue;
};

template<class Cmd>
class FixedVariableGroup: public FixedCommandGroup<Cmd>
{
public:
    FixedVariableGroup(const string& name,
                size_t maxCount, const any& initValue)
        : FixedCommandGroup<Cmd>(name, maxCount), m_initValue(initValue) {}

    const any& initValue() const { return m_initValue; }
    void setInitValue(const any& initValue) { m_initValue = initValue; }

    string groupType() const { return "register"; }
    Command::ptr createCommand(const string& name) {
        return Command::ptr(new Cmd(name, m_initValue));
    }

    /*shared_ptr<Node> parse(Parser& parser);
    bool execute(Parser& parser, shared_ptr<Node> node);
    */

protected:
    any m_initValue;
};

class RegisterGroupDef: public Command
{
public:
    RegisterGroupDef(const string& name, shared_ptr<CommandGroupBase> group)
        : Command(name), m_group(group) {}

    shared_ptr<CommandGroupBase> group() { return m_group; }
    bool parseArgs(Parser& parser, shared_ptr<Node> node);
    bool execute(Parser& parser, shared_ptr<Node> node);

protected:
    shared_ptr<CommandGroupBase> m_group;
};

} // namespace base
} // namespace texpp

#endif

