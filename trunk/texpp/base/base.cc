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
#include <texpp/base/dimen.h>
#include <texpp/base/glue.h>
#include <texpp/base/toks.h>
#include <texpp/base/font.h>

#include <texpp/parser.h>

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
    
    __TEXPP_SET_COMMAND("end",        End);
    __TEXPP_SET_COMMAND("relax",      Relax);
    __TEXPP_SET_COMMAND("par",        Relax);
    __TEXPP_SET_COMMAND("let",        Let);
    __TEXPP_SET_COMMAND("show",       Show);
    __TEXPP_SET_COMMAND("showthe",    ShowThe);
    __TEXPP_SET_COMMAND("message",    Message);

    __TEXPP_SET_COMMAND("advance",
        ArithmeticCommand, Variable::ADVANCE);
    __TEXPP_SET_COMMAND("multiply",
        ArithmeticCommand, Variable::MULTIPLY);
    __TEXPP_SET_COMMAND("divide",
        ArithmeticCommand, Variable::DIVIDE);

    shared_ptr<FontSelector> nullfont(
        new FontSelector("\\nullfont", "nullfont"));
    parser.setSymbol("\\nullfont", nullfont);

    #define __TEXPP_SET_FONT_GROUP(name, T) \
        parser.setSymbol("\\" name, Command::ptr(new T("\\" name))); \
        parser.setSymbol(name, nullfont)

    //__TEXPP_SET_FONT_GROUP("font", Font);

    __TEXPP_SET_COMMAND("catcode", CharcodeVariable, int(0), 0, 15);
    __TEXPP_SET_COMMAND("lccode", CharcodeVariable, int(0), 0, 255);
    __TEXPP_SET_COMMAND("uscode", CharcodeVariable, int(0), 0, 255);
    __TEXPP_SET_COMMAND("sfcode", CharcodeVariable, int(0), 0, 32767);
    __TEXPP_SET_COMMAND("mathcode", CharcodeVariable, int(0), 0, 32768);
    __TEXPP_SET_COMMAND("delcode", CharcodeVariable, int(0),
                                        TEXPP_INT_MIN, 16777215);

    __TEXPP_SET_COMMAND("count", CountRegister, int(0));
    __TEXPP_SET_COMMAND("dimen", DimenRegister, int(0));
    __TEXPP_SET_COMMAND("skip", GlueRegister, Glue(0));
    __TEXPP_SET_COMMAND("muskip", MuGlueRegister, Glue(0));
    __TEXPP_SET_COMMAND("toks", ToksRegister, Token::list());

    #define __TEXPP_SET_REGDEF(name, T, I) \
        parser.setSymbol("\\" name "def", \
            Command::ptr(new RegisterDef<T, I>("\\" name "def",  \
                static_pointer_cast<T>( \
                    parser.symbol("\\" name, Command::ptr())))))
        
    __TEXPP_SET_REGDEF("count", CountRegister, IntegerVariable);
    __TEXPP_SET_REGDEF("dimen", DimenRegister, DimenVariable);
    __TEXPP_SET_REGDEF("skip", GlueRegister, GlueVariable);
    __TEXPP_SET_REGDEF("muskip", MuGlueRegister, MuGlueVariable);
    __TEXPP_SET_REGDEF("toks", ToksRegister, ToksVariable);

    #define __TEXPP_SET_VARIABLE(name, T, value, ...) \
        parser.setSymbol("\\" name, \
            Command::ptr(new T("\\" name, value, ##__VA_ARGS__))); \
        parser.setSymbol(name, value)

    __TEXPP_SET_VARIABLE("endlinechar", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("mag", IntegerVariable, int(0));

    __TEXPP_SET_VARIABLE("pretolerance", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tolerance", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("hbadness", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("vbadness", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("linepenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("hyphenpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("exhyphenpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("binoppenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("relpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("clubpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("widowpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("displaywidowpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("brokenpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("predisplaypenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("postdisplaypenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("interlinepenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("floatingpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("outputpenalty", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("doublehyphendemerits", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("finalhyphendemerits", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("adjdemerits", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("looseness", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("pausing", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("holdinginserts", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingonline", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingmacros", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingstats", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingparagraphs", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingpages", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingoutput", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracinglostchars", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingcommands", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("tracingrestores", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("language", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("uchyph", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("lefthyphenmin", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("righthyphenmin", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("globaldefs", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("defaulthyphenchar", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("defaultskewchar", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("escapechar", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("newlinechar", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("maxdeadcycles", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("hangafter", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("fam", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("delimiterfactor", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("time", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("day", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("month", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("year", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("showboxbreadth", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("showboxdepth", IntegerVariable, int(0));
    __TEXPP_SET_VARIABLE("errorcontextlines", IntegerVariable, int(0));

    __TEXPP_SET_VARIABLE("hfuzz", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("vfuzz", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("overfullrule", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("emergencystretch", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("hsize", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("vsize", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("maxdepth", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("splitmaxdepth", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("boxmaxdepth", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("lineskiplimit", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("delimitershortfall", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("nulldelimiterspace", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("scriptspace", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("mathsurround", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("predisplaysize", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("displaywidth", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("displayindent", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("parindent", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("hangindent", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("hoffset", DimenVariable, int(0));
    __TEXPP_SET_VARIABLE("voffset", DimenVariable, int(0));

    __TEXPP_SET_VARIABLE("baselineskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("lineskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("parskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("abovedisplayskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("abovedisplayshortskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("belowdisplayskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("belowdisplayshortskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("leftskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("rightskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("topskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("splittopskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("tabskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("spaceskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("xspaceskip", GlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("parfillskip", GlueVariable, Glue(0));

    __TEXPP_SET_VARIABLE("thinmuskip", MuGlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("medmuskip", MuGlueVariable, Glue(0));
    __TEXPP_SET_VARIABLE("thickmuskip", MuGlueVariable, Glue(0));

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

