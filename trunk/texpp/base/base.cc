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

#include <texpp/base/base.h>

#include <texpp/base/misc.h>
#include <texpp/base/show.h>
#include <texpp/base/func.h>
#include <texpp/base/files.h>
#include <texpp/base/variables.h>

namespace texpp {
namespace base {

void initSymbols(Parser& parser)
{
    #define __TEXPP_SET_COMMAND(name, T, ...) \
        parser.setSymbol(name, Command::ptr(new T(name, ##__VA_ARGS__)))
    
    #define __TEXPP_SET_VARIABLE(name, value) \
        parser.setSymbol(name, value)

    __TEXPP_SET_COMMAND("\\relax",      Relax);
    __TEXPP_SET_COMMAND("\\par",        Relax);
    __TEXPP_SET_COMMAND("\\let",        Let);
    __TEXPP_SET_COMMAND("\\show",       Show);
    __TEXPP_SET_COMMAND("\\message",    Message);

    __TEXPP_SET_COMMAND("\\e",IntegerVariable);
}

} // namespace base
} // namespace texpp

