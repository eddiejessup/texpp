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

#ifndef __TEXPP_BASE_COMMANDGROUP_H
#define __TEXPP_BASE_COMMANDGROUP_H

#include <texpp/common.h>
#include <texpp/parser.h>

#include <sstream>

namespace texpp {
namespace base {

class CommandGroupBase: public Command
{
public:
    CommandGroupBase(const string& name) : Command(name) {}
    virtual Command::ptr item(size_t n) = 0;
    virtual Command::ptr createCommand(const string& name) = 0;

    virtual string groupType() const { return "group"; }

    bool parseArgs(Parser& parser, Node::ptr node);
    bool execute(Parser& parser, Node::ptr node);
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
        std::ostringstream newname;
        newname << name() << n;
        m_items[n] = createCommand(newname.str());
    }
    return m_items[n];
}

} // namespace base
} // namespace texpp


#endif


