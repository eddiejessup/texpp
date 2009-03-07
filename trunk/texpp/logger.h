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

#ifndef __TEXPP_LOGGER_H
#define __TEXPP_LOGGER_H

#include <texpp/common.h>

namespace texpp {

class Token;
class Parser;

class Logger
{
public:
    enum { MESSAGE = 0, SHOW = 30,
           ERROR = 40, CRITICAL = 50 };

    Logger() {}
    virtual ~Logger() {}

    const string& levelName(int level) const;
    string tokenLines(Parser& parser, shared_ptr<Token> token) const;

    virtual bool log(int level, const string& message,
                    Parser& parser, shared_ptr<Token> token) = 0;
};

class NullLogger: public Logger
{
public:
    bool log(int, const string&, Parser&, shared_ptr<Token>) { return true; }
};

class ConsoleLogger: public Logger
{
public:
    bool log(int level, const string& message,
                Parser& parser, shared_ptr<Token> token);
};

} // namespace texpp

#endif

