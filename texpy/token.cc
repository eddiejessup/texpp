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

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

void export_token_class()
{
    using namespace boost::python;
    using namespace texpp;

    scope scope_Token = class_<Token, shared_ptr<Token> >(
            "Token", init<Token::Type, Token::CatCode, const string&,
                    const string&, size_t, size_t, size_t>())
        .def(init<Token::Type, Token::CatCode,
                    const string&, const string&, size_t, size_t>())
        .def(init<Token::Type, Token::CatCode, const string&,
                    const string&, size_t>())
        .def(init<Token::Type, Token::CatCode, const string&,
                    const string&>())
        .def(init<Token::Type, Token::CatCode, const string&>())

        .def("__repr__", &Token::repr)

        .add_property("type", &Token::type, &Token::setType)
        .add_property("catCode", &Token::catCode, &Token::setCatCode)
        .add_property("value", make_function(&Token::value,
                    return_value_policy<copy_const_reference>()),
                    &Token::setValue)
        .add_property("source", make_function(&Token::source,
                    return_value_policy<copy_const_reference>()),
                    &Token::setSource)
        .add_property("lineNo", &Token::lineNo, &Token::setLineNo)
        .add_property("charPos", &Token::charPos, &Token::setCharPos)
        .add_property("charEnd", &Token::charEnd, &Token::setCharEnd)
        .def("isSkipped", &Token::isSkipped)
        .def("isControl", &Token::isControl)
        .def("isCharacter", (bool (Token::*)() const) &Token::isSkipped)
        .def("isCharacter", (bool (Token::*)(char) const) &Token::isCharacter)
        .def("isCharacter", (bool (Token::*)(char, Token::CatCode) const)
                                    &Token::isCharacter)
        .def("isCharacterCat", (bool (Token::*)(Token::CatCode) const)
                                    &Token::isCharacterCat)
        .def("isLastInLine", &Token::isLastInLine)
        .def("texRepr", &Token::texRepr)
        .def("meaning", &Token::meaning)
        ;

    enum_<Token::Type>("Type")
        .value("TOK_SKIPPED", Token::TOK_SKIPPED)
        .value("TOK_CHARACTER", Token::TOK_CHARACTER)
        .value("TOK_CONTROL", Token::TOK_CONTROL)
        ;

    enum_<Token::CatCode>("CatCode")
        .value("CC_ESCAPE", Token::CC_ESCAPE)
        .value("CC_BGROUP", Token::CC_BGROUP)
        .value("CC_EGROUP", Token::CC_EGROUP)
        .value("CC_MATHSHIFT", Token::CC_MATHSHIFT)
        .value("CC_ALIGNTAB", Token::CC_ALIGNTAB)
        .value("CC_EOL", Token::CC_EOL)
        .value("CC_PARAM", Token::CC_PARAM)
        .value("CC_SUPER", Token::CC_SUPER)
        .value("CC_SUB", Token::CC_SUB)
        .value("CC_IGNORED", Token::CC_IGNORED)
        .value("CC_SPACE", Token::CC_SPACE)
        .value("CC_LETTER", Token::CC_LETTER)
        .value("CC_OTHER", Token::CC_OTHER)
        .value("CC_ACTIVE", Token::CC_ACTIVE)
        .value("CC_COMMENT", Token::CC_COMMENT)
        .value("CC_INVALID", Token::CC_INVALID)
        .value("CC_NONE", Token::CC_NONE)
        ;

}

void export_token()
{
    using namespace boost::python;
    using namespace texpp;
    export_token_class();

    class_<std::vector< shared_ptr<Token> > >("TokenList")
        .def(vector_indexing_suite<std::vector< shared_ptr<Token> >, true >())
    ;
}

