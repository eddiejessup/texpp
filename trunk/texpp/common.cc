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

#include <texpp/common.h>
#include <texpp/token.h>
#include <texpp/parser.h>

#include <sstream>
#include <iomanip>
#include <cassert>
#include <exception>

#include <boost/any.hpp>

namespace texpp {

string reprString(const string& s)
{
    std::ostringstream r;
    r << '\"';
    texpp::string::const_iterator end = s.end();
    for(texpp::string::const_iterator c = s.begin(); c != end; ++c) {
        if(*c == '\n') r << "\\n";
        else if(*c == '\r') r << "\\r";
        else if(*c == '\t') r << "\\t";
        else if(*c == '\0') r << "\\0";
        else if(*c == '\"') r << "\\\"";
        else if(*c == '\\') r << "\\\\";
        else if(*c < 0x20 || *c >= 0x7f)
            r << "\\x" << std::setfill('0') << std::setw(2)
                       << std::hex << (unsigned int)(*c);
        else
            r << *c;
    }
    r << '\"';
    return r.str();
}

string reprAny(const boost::any& value)
{
    using boost::unsafe_any_cast;

    std::ostringstream r;
    if(value.empty())
        r << "any()";
    else if(value.type() == typeid(int))
        r << *unsafe_any_cast<int>(&value);
    else if(value.type() == typeid(short))
        r << *unsafe_any_cast<short>(&value);
    else if(value.type() == typeid(long))
        r << *unsafe_any_cast<long>(&value);
    else if(value.type() == typeid(string))
        r << reprString(*unsafe_any_cast<string>(&value));
    else if(value.type() == typeid(Token::ptr))
        r << (*unsafe_any_cast<Token::ptr>(&value))->repr();
    else if(value.type() == typeid(Command::ptr))
        r << (*unsafe_any_cast<Command::ptr>(&value))->repr();
    else if(value.type() == typeid(Token::list_ptr))
        r << "TokenList("
          << (*unsafe_any_cast<Token::list_ptr>(&value))->size()
          << " tokens)";
    else
        r << "any(" << value.type().name() << "())";
    return r.str();
}


} // namespace texpp

namespace boost {

void throw_exception(std::exception const &) {
    assert(false);
}

} // namespace boost

