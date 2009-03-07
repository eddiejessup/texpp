/* This file is part of texpp.
   Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

   texpp is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   texpp is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with texpp.  If not, see <http://www.gnu.org/licenses/>.
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

    Node::ptr parse(Parser& parser);
    bool execute(Parser&, Node::ptr);
};

class FutureLet: public Command
{
public:
    explicit FutureLet(const string& name): Command(name) {}

    Node::ptr parse(Parser& parser);
    bool execute(Parser&, Node::ptr);
};

} // namespace base
} // namespace texpp


#endif

