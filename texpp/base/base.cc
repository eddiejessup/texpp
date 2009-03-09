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
#include <texpp/base/commandgroup.h>

namespace texpp {
namespace base {

void initSymbols(Parser& parser)
{
    #define __TEXPP_SET_COMMAND(name, T, ...) \
        parser.setSymbol("\\" name, Command::ptr(new T(name, ##__VA_ARGS__)))
    
    #define __TEXPP_SET_VARIABLE(name, value, T, ...) \
        parser.setSymbol("\\" name, Command::ptr(new T(name, ##__VA_ARGS__))); \
        parser.setSymbol(name, value)

    #define __TEXPP_SET_VARIABLE_GROUP(name, value, maxcount, T) \
        parser.setSymbol("\\" name, Command::ptr( \
            new FixedVariableGroup<T>(name, maxcount, value))); \
        parser.setSymbol(name, value)

    __TEXPP_SET_COMMAND("relax",      Relax);
    __TEXPP_SET_COMMAND("par",        Relax);
    __TEXPP_SET_COMMAND("let",        Let);
    __TEXPP_SET_COMMAND("show",       Show);
    __TEXPP_SET_COMMAND("message",    Message);

    __TEXPP_SET_VARIABLE_GROUP("count", int(0), 256, IntegerVariable);

    __TEXPP_SET_VARIABLE("endlinechar", int(0), EndlinecharVariable);

    __TEXPP_SET_VARIABLE("pretolerance", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tolerance", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("hbadness", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("vbadness", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("linepenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("hyphenpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("exhyphenpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("binoppenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("relpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("clubpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("widowpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("displaywidowpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("brokenpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("predisplaypenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("postdisplaypenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("interlinepenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("floatingpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("outputpenalty", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("doublehyphendemerits", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("finalhyphendemerits", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("adjdemerits", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("looseness", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("pausing", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("holdinginserts", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingonline", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingmacros", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingstats", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingparagraphs", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingpages", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingoutput", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracinglostchars", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingcommands", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("tracingrestores", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("language", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("uchyph", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("lefthyphenmin", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("righthyphenmin", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("globaldefs", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("defaulthyphenchar", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("defaultskewchar", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("escapechar", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("newlinechar", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("maxdeadcycles", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("hangafter", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("fam", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("mag", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("delimiterfactor", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("time", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("day", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("month", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("year", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("showboxbreadth", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("showboxdepth", int(0), IntegerVariable);
    __TEXPP_SET_VARIABLE("errorcontextlines", int(0), IntegerVariable);



}

} // namespace base
} // namespace texpp

