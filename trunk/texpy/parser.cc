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

#include <boost/python.hpp>
#include <texpp/parser.h>

#include <boost/any.hpp>
#include <memory>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "std_pair.h"

namespace texpp { namespace {

using boost::python::object;

class ParserWrap: public Parser {
public:
    ParserWrap(const string& fileName, const std::auto_ptr<std::istream>& file,
            bool interactive = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>())
        : Parser(fileName, const_cast<std::auto_ptr<std::istream>&>(file),
                    interactive, logger) {}

    Token::ptr peekToken0() { return Parser::peekToken(); }
    Token::ptr nextToken0() { return Parser::nextToken(); }

    const any& symbol0(const string& name) const {
        return Parser::symbolAny(name);
    }
    const any& symbol1(Token::ptr token) const {
        return Parser::symbolAny(token);
    }
    void setSymbol0(const string& name, const any& value) {
        Parser::setSymbol(name, value);
    }
    void setSymbol1(Token::ptr token, const any& value) {
        Parser::setSymbol(token, value);
    }
};

}}

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
    Node_treeRepr_overloads, treeRepr, 0, 1)

void export_node()
{
    using namespace boost::python;
    using namespace texpp;

    export_std_pair<string, Node::ptr>();

    scope scopeNode = class_<Node, shared_ptr<Node> >(
            "Node", init<std::string>())
        .def("__repr__", &Node::repr)
        .def("treeRepr", &Node::treeRepr,
            Node_treeRepr_overloads())
        .def("source", &Node::source)
        .def("type", &Node::type,
            return_value_policy<copy_const_reference>())
        .def("setValue", &Node::setValue)
        .def("value", &Node::valueAny,
            return_value_policy<return_by_value>())
        .def("tokens", (vector<Token::ptr>& (Node::*)())(&Node::tokens),
            return_internal_reference<1,
                return_value_policy<reference_existing_object> >())
        .def("children", (Node::ChildrenList& (Node::*)())(&Node::children),
            return_internal_reference<1,
                return_value_policy<reference_existing_object> >())
        .def("appendChild", &Node::appendChild)
        .def("child", (Node::ptr (Node::*)(const string&))(&Node::child))
        ;

    class_<Node::ChildrenList>("ChildrenList")
        .def(vector_indexing_suite<Node::ChildrenList, true >())
    ;

}

void export_parser()
{
    using namespace boost::python;
    using namespace texpp;
    using boost::any;

    export_node();

    class_<ParserWrap, boost::noncopyable >("Parser",
            init<std::string, const std::auto_ptr<std::istream>&, bool>())
        .def(init<std::string, const std::auto_ptr<std::istream>&>())

        .def("parse", &Parser::parse)

        // Tokens
        .def("peekToken", &Parser::peekToken)
        .def("peekToken", &ParserWrap::peekToken0)
        .def("nextToken", &Parser::nextToken)
        .def("nextToken", &ParserWrap::nextToken0)

        // Symbols
        .def("symbol", (const any& (Parser::*)(const string&, bool) const)(
                &Parser::symbolAny), return_value_policy<return_by_value>())
        .def("symbol", (const any& (Parser::*)(Token::ptr, bool) const)(
                &Parser::symbolAny), return_value_policy<return_by_value>())
        .def("symbol", &ParserWrap::symbol0,
                return_value_policy<return_by_value>())
        .def("symbol", &ParserWrap::symbol1,
                return_value_policy<return_by_value>())
        .def("setSymbol", (void (Parser::*)(const string&, const any&, bool))
                        (&Parser::setSymbol))
        .def("setSymbol", (void (Parser::*)(Token::ptr, const any&, bool))
                        (&Parser::setSymbol))
        .def("setSymbol", &ParserWrap::setSymbol0)
        .def("setSymbol", &ParserWrap::setSymbol1)
        .def("beginGroup", &Parser::beginGroup)
        .def("endGroup", &Parser::endGroup)

        // Parse helpers
        .def("parseCommandArgs", &Parser::parseCommandArgs)
        .def("executeCommand", &Parser::executeCommand)
        ;

}

