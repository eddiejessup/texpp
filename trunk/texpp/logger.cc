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

#include <texpp/logger.h>
#include <texpp/token.h>
#include <texpp/lexer.h>
#include <texpp/parser.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
texpp::string loggerLevelNames[] = {
    "message", "show", "error", "critical"
};
const int MAX_LINE_CHARS = 1000;
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
        string line1 = line.substr(0, token->charEnd());
        if(!line1.empty() && line1[line1.size()-1] == '\n')
            line1 = line1.substr(0, line1.size()-1);
        r << line1 << '\n';
        r << string(r.str().size()-1, ' ');
        r << line.substr(token->charEnd(),
                    line.find_last_not_of(" \r\n") + 1 - token->charEnd());
        r << '\n';
    }

    return r.str();
}

ConsoleLogger::~ConsoleLogger()
{
    if(!m_atNewline) std::cout << std::endl;
}

bool ConsoleLogger::log(int level, const string& message,
                            Parser& parser, Token::ptr token)
{
    std::ostringstream r;
    
    if(level <= MESSAGE) {
        if(!m_atNewline) std::cout << ' '; 
        r << message;
    } else {
        if(!m_atNewline) std::cout << std::endl;
        if(level <= SHOW) r << "> " << message << ".\n";
        else r << "! " << message << ".\n";
        if(token && token->lineNo())
            r << tokenLines(parser, token) << "\n";
    }

    string msg(r.str());
    int newlinechar = parser.symbol("newlinechar", int(0));
    if(newlinechar >= 0 && newlinechar < 256)
        std::replace(msg.begin(), msg.end(), char(newlinechar), '\n');

    std::cout << msg;

    if(!msg.empty()) m_atNewline = msg[msg.size()-1] == '\n';

    if(parser.lexer()->interactive() && !m_atNewline) {
        std::cout << std::endl;
        m_atNewline = true;
    }

    return true;
}

} // namespace texpp

