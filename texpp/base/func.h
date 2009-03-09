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
#include <texpp/parser.h>

namespace texpp {
namespace base {

class Let: public Command
{
public:
    explicit Let(const string& name): Command(name) {}

    bool parseArgs(Parser& parser, Node::ptr node);
    bool execute(Parser&, Node::ptr);
};

class FutureLet: public Command
{
public:
    explicit FutureLet(const string& name): Command(name) {}

    bool parseArgs(Parser& parser, Node::ptr node);
    bool execute(Parser&, Node::ptr);
};

} // namespace base
} // namespace texpp


#endif

