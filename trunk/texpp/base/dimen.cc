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

#include <texpp/base/dimen.h>
#include <texpp/parser.h>

#include <sstream>

namespace texpp {
namespace base {

bool InternalDimen::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("equals", parser.parseOptionalEquals(false));
    node->appendChild("rvalue", parser.parseDimen());
    return check(parser, node->child("rvalue"));
}

bool InternalDimen::execute(Parser& parser, Node::ptr node)
{
    return set(parser, node->child("rvalue")->value(int(0)));
}

tuple<int,int,bool> InternalDimen::multiplyIntFrac(int x, int n, int d)
{
    int sign = 1;
    if(x < 0) { x=-x; sign=-1; }
    unsigned int t = (x % 0x8000) * n;
    unsigned int u = (x / 0x8000) * n + (t / 0x8000);
    unsigned int v = (u % d) * 0x8000 + (t % 0x8000);
    if(u / d >= 0x8000) // XXX: error
        return boost::make_tuple(0, 0, true);
    return boost::make_tuple(sign * int((u / d) * 0x8000 + (v / d)),
                    sign * (v % d), false);
}

string InternalDimen::dimenToString(int n, int o)
{
    std::ostringstream s;
    if(n<0) { s << '-'; n=-n; }

    s << n/0x10000 << '.';

    n = 10*(n & 0xffff) + 5;
    int delta = 10;
    do {
        if(delta > 0x10000) n+=0x8000-50000;
        s << char('0' + n/0x10000);
        n = 10*(n & 0xffff);
        delta *= 10;
    } while(n > delta);

    if(o == 0) s << "pt";
    else if(o == 1) s << "fil";
    else if(o == 2) s << "fill";
    else if(o == 3) s << "filll";
    return s.str();
}

} // namespace base
} // namespace texpp

