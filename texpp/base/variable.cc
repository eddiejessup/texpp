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

#include <texpp/base/variable.h>
#include <texpp/parser.h>
#include <texpp/logger.h>
#include <texpp/lexer.h>

#include <iostream>

namespace texpp {
namespace base {

const any& Variable::getAny(Parser& parser, bool global)
{
    const any& ret = parser.symbolAny(name().substr(1), global);
    return !ret.empty() ? ret : m_initValue;
}

bool Variable::set(Parser& parser, const any& value, bool global)
{
    string varname = name().substr(1);
    parser.setSymbol(varname, value, global);
    return true;
    /*
    any s = parser.symbolAny(varname, global);
    if(s.type() == value.type()) {
        parser.setSymbol(varname, value, global);
        return true;
    } else {
        return false;
    }*/
}

} // namespace base
} // namespace texpp

