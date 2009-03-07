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

#include <texpp/logger.h>
#include <texpp/token.h>
#include <texpp/lexer.h>
#include <texpp/parser.h>

#include <iostream>
#include <sstream>

namespace {
texpp::string loggerLevelNames[] = {
    "message", "show", "error", "critical"
};
} // namespace

namespace texpp {

const string& Logger::levelName(int level) const
{
    if(level <= MESSAGE) return loggerLevelNames[0];
    else if(level <= SHOW) return loggerLevelNames[1];
    else if(level <= ERROR) return loggerLevelNames[2];
    else return loggerLevelNames[3];
}

string Logger::tokenLines(Parser& parser, Token::ptr token) const
{
    if(!token || !token->lineNo()) return string();

    std::ostringstream r;
    if(parser.lexer()->fileName().empty()) r << "<*> ";
    else r << "l." << token->lineNo() << " ";

    const string& line = parser.lexer()->line(token->lineNo());
    if(!line.empty()) {
        r << line.substr(0, token->charEnd()) << '\n';
        r << string(r.str().size()-1, ' ');
        r << line.substr(token->charEnd(),
                    line.find_last_not_of(" \r\n") + 1 - token->charEnd());
    }

    return r.str();
}

bool ConsoleLogger::log(int level, const string& message,
                            Parser& parser, Token::ptr token)
{
    if(level <= MESSAGE) {
        std::cout << message << "\n";
    } else {
        if(level <= SHOW)
            std::cout << "> " << message << ".\n";
        else
            std::cout << "! " << message << ".\n";
        if(token && token->lineNo())
            std::cout << tokenLines(parser, token) << "\n";
    }
    return true;
}

} // namespace texpp

