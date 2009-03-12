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

#ifndef __TEXPP_BASE_DIMEN_H
#define __TEXPP_BASE_DIMEN_H

#include <texpp/common.h>
#include <texpp/command.h>

#include <texpp/base/variable.h>
#include <boost/tuple/tuple.hpp>

namespace texpp {

using boost::tuple;

namespace base {

class InternalDimen: public Variable
{
public:
    InternalDimen(const string& name, const any& initValue = any(0))
        : Variable(name, initValue) {}
    bool parseArgs(Parser& parser, shared_ptr<Node> node);
    bool execute(Parser& parser, shared_ptr<Node> node);

    static tuple<int,int,bool> multiplyIntFrac(int x, int n, int d);
    static string scaledToString(int n);
};

class DimenVariable: public InternalDimen
{
public:
    DimenVariable(const string& name, const any& initValue = any(0))
        : InternalDimen(name, initValue) {}
};

} // namespace base
} // namespace texpp

#endif

