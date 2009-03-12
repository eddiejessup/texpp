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

#include <texpp/base/glue.h>
#include <texpp/base/dimen.h>
#include <texpp/parser.h>

#include <sstream>

namespace texpp {
namespace base {

bool InternalGlue::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("equals", parser.parseOptionalEquals(false));
    node->appendChild("rvalue", parser.parseGlue());
    return check(parser, node->child("rvalue"));
}

bool InternalGlue::execute(Parser& parser, Node::ptr node)
{
    return set(parser, node->child("rvalue")->value(Glue(0)));
}

string InternalGlue::glueToString(const Glue& g)
{
    string s = InternalDimen::dimenToString(g.width);
    if(g.stretch) {
        s = s + " plus " +
            InternalDimen::dimenToString(g.stretch, g.stretchOrder);
    }
    if(g.shrink) {
        s = s + " minus " +
            InternalDimen::dimenToString(g.shrink, g.shrinkOrder);
    }
    return s;
}

} // namespace base
} // namespace texpp


