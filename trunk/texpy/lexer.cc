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
#include <texpp/lexer.h>

#include <memory>

namespace texpp {
class Lexer_: public Lexer {
public:
    Lexer_(const string& fileName, const std::auto_ptr<std::istream>& file,
                    bool interactive = false, bool saveLines = false)
        : Lexer(fileName, const_cast<std::auto_ptr<std::istream>&>(file),
                        interactive, saveLines) {}
};
}

void export_lexer()
{
    using namespace boost::python;
    using namespace texpp;

    class_<Lexer_, std::auto_ptr<Lexer_> >("Lexer",
            init<std::string, const std::auto_ptr<std::istream>&,bool,bool>())
        .def(init<std::string, const std::auto_ptr<std::istream>&,bool>())
        .def(init<std::string, const std::auto_ptr<std::istream>&>())
        .def("nextToken", &Lexer::nextToken)
        .def("fileName", &Lexer::fileName, 
                return_value_policy<copy_const_reference>())
        .def("line", (const string& (Lexer::*)() const) &Lexer::line,
                return_value_policy<copy_const_reference>())
        .def("line", (const string& (Lexer::*)(size_t) const) &Lexer::line,
                return_value_policy<copy_const_reference>())
        .def("lineNo", &Lexer::lineNo)
        .def("endlinechar", &Lexer::endlinechar)
        .def("setEndlinechar", &Lexer::setEndlinechar)
        .def("catcode", &Lexer::catcode)
        .def("setCatcode", &Lexer::setCatcode)
        ;
}

