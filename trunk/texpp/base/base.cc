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
#include <texpp/base/variable.h>
#include <texpp/base/integer.h>
#include <texpp/base/commandgroup.h>

#include <boost/lexical_cast.hpp>
#include <ctime>
#include <climits>

namespace texpp {
namespace base {

void initSymbols(Parser& parser)
{
    #define __TEXPP_SET_COMMAND(name, T, ...) \
        parser.setSymbol("\\" name, \
            Command::ptr(new T("\\" name, ##__VA_ARGS__)))
    
    #define __TEXPP_SET_VARIABLE(name, value, T, ...) \
        parser.setSymbol("\\" name, \
            Command::ptr(new T("\\" name, ##__VA_ARGS__))); \
        parser.setSymbol(name, value)

    #define __TEXPP_SET_VARIABLE_GROUP(name, value, maxcount, T) \
        parser.setSymbol("\\" name, Command::ptr( \
            new FixedVariableGroup<T>("\\" name, maxcount, value))); \
        parser.setSymbol(name, value)

    #define __TEXPP_SET_CHARCODE_GROUP(name, value, maxcount, T, MIN, MAX) \
        parser.setSymbol("\\" name, Command::ptr( \
            new CharcodeVariableGroup< T, MIN, MAX >( \
                "\\" name, maxcount, value))); \
        parser.setSymbol(name, value)


    __TEXPP_SET_COMMAND("end",        End);
    __TEXPP_SET_COMMAND("relax",      Relax);
    __TEXPP_SET_COMMAND("par",        Relax);
    __TEXPP_SET_COMMAND("let",        Let);
    __TEXPP_SET_COMMAND("show",       Show);
    __TEXPP_SET_COMMAND("showthe",    ShowThe);
    __TEXPP_SET_COMMAND("message",    Message);

    __TEXPP_SET_VARIABLE_GROUP("count", int(0), 256, IntegerVariable);

    __TEXPP_SET_CHARCODE_GROUP("catcode", int(0), 256,
                                CatcodeVariable, 0, 15);
    __TEXPP_SET_CHARCODE_GROUP("lccode", int(0), 256,
                                CharcodeVariable, 0, 255);
    __TEXPP_SET_CHARCODE_GROUP("uccode", int(0), 256,
                                CharcodeVariable, 0, 255);
    __TEXPP_SET_CHARCODE_GROUP("sfcode", int(0), 256,
                                CharcodeVariable, 0, 32767);
    __TEXPP_SET_CHARCODE_GROUP("mathcode", int(0), 256,
                                CharcodeVariable, 0, 32768);
    __TEXPP_SET_CHARCODE_GROUP("delcode", int(0), 256,
                                CharcodeVariable, INT_MIN, 16777215);

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

    for(int i=0; i<256; ++i) {
        string n = boost::lexical_cast<string>(i);

        parser.lexer()->setCatcode(i, Token::CC_OTHER);
        parser.setSymbol("catcode"+n, int(Token::CC_OTHER));

        parser.setSymbol("delcode"+n, int(-1));
        parser.setSymbol("mathcode"+n, int(i));
    }

    for(int i='a'; i<='z'; ++i) {
        string n = boost::lexical_cast<string>(i);

        parser.lexer()->setCatcode(i, Token::CC_LETTER);
        parser.setSymbol("catcode"+n, int(Token::CC_LETTER));

        parser.setSymbol("sfcode"+n, int(1000));
        parser.setSymbol("lccode"+n, int(i));
        parser.setSymbol("uccode"+n, int(i - 'a' + 'A'));
        parser.setSymbol("mathcode"+n, int(0x7100 + i));
    }

    for(int i='A'; i<='Z'; ++i) {
        string n = boost::lexical_cast<string>(i);

        parser.lexer()->setCatcode(i, Token::CC_LETTER);
        parser.setSymbol("catcode"+n, int(Token::CC_LETTER));

        parser.setSymbol("sfcode"+n, int(999));
        parser.setSymbol("lccode"+n, int(i + 'a' - 'A'));
        parser.setSymbol("uccode"+n, int(i));
        parser.setSymbol("mathcode"+n, int(0x7100 + i));
    }

    for(int i='0'; i<='9'; ++i) {
        string n = boost::lexical_cast<string>(i);
        parser.setSymbol("mathcode"+n, int(0x7000 + i));
    }

    parser.lexer()->setCatcode(0x7f,   Token::CC_INVALID);
    parser.setSymbol("catcode127", int(Token::CC_INVALID));
    parser.lexer()->setCatcode('\\',   Token::CC_ESCAPE);
    parser.setSymbol("catcode92",  int(Token::CC_ESCAPE));
    parser.lexer()->setCatcode('\r',   Token::CC_EOL);
    parser.setSymbol("catcode13",  int(Token::CC_EOL));
    parser.lexer()->setCatcode(' ',    Token::CC_SPACE);
    parser.setSymbol("catcode32",  int(Token::CC_SPACE));
    parser.lexer()->setCatcode('%',    Token::CC_COMMENT);
    parser.setSymbol("catcode37",  int(Token::CC_COMMENT));

    parser.setSymbol("delcode96", int(0));

    parser.lexer()->setEndlinechar('\r');
    parser.setSymbol("endlinechar", int('\r'));

    parser.setSymbol("escapechar", int('\\'));
    parser.setSymbol("tolerance", int(10000));
    parser.setSymbol("mag", int(1000));
    parser.setSymbol("maxdeadcycles", int(25));

    std::time_t t; std::time(&t);
    std::tm* time = std::localtime(&t);
    parser.setSymbol("year", int(1900+time->tm_year));
    parser.setSymbol("month", int(1+time->tm_mon));
    parser.setSymbol("day", int(time->tm_mday));
    parser.setSymbol("time", int(time->tm_hour*60 + time->tm_min));
}

} // namespace base
} // namespace texpp

