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

#include <boost/python.hpp>
#include <texpp/token.h>

#include <iostream>
#include <istream>
#include <memory>

void export_python_stream();
void export_boost_any();
void export_token();
void export_lexer();
void export_command();
void export_parser();


BOOST_PYTHON_MODULE(texpy)
{
    using namespace boost::python;
    using namespace texpp;

    export_python_stream();
    export_boost_any();
    export_token();
    export_lexer();
    export_command();
    export_parser();
}

