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

#ifndef __TEXPP_BASE_GLUE_H
#define __TEXPP_BASE_GLUE_H

#include <texpp/common.h>
#include <texpp/command.h>

#include <texpp/base/variable.h>
#include <boost/tuple/tuple.hpp>

namespace texpp {

namespace base {

struct Glue
{
    int width;
    int stretch, stretchOrder;
    int shrink, shrinkOrder;

    Glue() {}
    explicit Glue(int w, int st = 0, int sto = 0, int sh = 0, int sho = 0)
        : width(w), stretch(st), stretchOrder(sto),
          shrink(sh), shrinkOrder(sho) {}
};

class InternalGlue: public Variable
{
public:
    InternalGlue(const string& name, const any& initValue = any(Glue(0)))
        : Variable(name, initValue) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);

    static string glueToString(const Glue& g, bool mu = false);
};

class GlueVariable: public InternalGlue
{
public:
    GlueVariable(const string& name, const any& initValue = any(Glue(0)))
        : InternalGlue(name, initValue) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);
};

class InternalMuGlue: public Variable
{
public:
    InternalMuGlue(const string& name, const any& initValue = any(Glue(0)))
        : Variable(name, initValue) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);

    static string glueToString(const Glue& g, bool mu = true) {
        return InternalGlue::glueToString(g, mu);
    }
};

class MuGlueVariable: public InternalMuGlue
{
public:
    MuGlueVariable(const string& name, const any& initValue = any(Glue(0)))
        : InternalMuGlue(name, initValue) {}

    bool invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op, bool global);
};

} // namespace base
} // namespace texpp

#endif


