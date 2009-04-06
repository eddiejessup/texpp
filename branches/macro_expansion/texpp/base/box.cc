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

#include <texpp/base/box.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {
namespace base {

bool Box::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN || op == GET) {
        string name = parseName(parser, node);
        Token::list tokens = parser.symbol(name, Token::list());
        node->setValue(tokens);
        return true;
    }
    return false;
}

bool Lastbox::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(parser.mode() == Parser::VERTICAL ||
                parser.mode() == Parser::MATH) {
        parser.logger()->log(Logger::ERROR,
            "You can't use `" + texRepr() + "' in " +
            parser.modeName() + " mode",
            parser, parser.lastToken());
        return true;
    }
    return Box::invokeOperation(parser, node, op);
}

string Vsplit::parseName(Parser& parser, shared_ptr<Node> node)
{
    string s = Register<Box>::parseName(parser, node);
    static vector<string> kw_to(1, "to");
    Node::ptr to = parser.parseKeyword(kw_to);
    if(!to) {
        parser.logger()->log(Logger::ERROR,
            "Missing `to' inserted",
            parser, parser.lastToken());
        to = Node::ptr(new Node("error_missing_to"));
        to->setValue(string("to"));
    }
    node->appendChild("to", to);
    node->appendChild("dimen", parser.parseDimen());
    return s;
}

string Setbox::parseName(Parser& parser, shared_ptr<Node> node)
{
    shared_ptr<Node> number = parser.parseNumber();
    node->appendChild("variable_number", number);
    int n = number->value(int(0));

    if(n < 0 || n > 255) {
        parser.logger()->log(Logger::ERROR,
            "Bad register code (" + boost::lexical_cast<string>(n) + ")",
            parser, parser.lastToken());
        n = 0;
    }

    return this->name().substr(1) + boost::lexical_cast<string>(n);
}

bool Setbox::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN) {
        string name = parseName(parser, node);

        node->appendChild("equals", parser.parseOptionalEquals());
        node->appendChild("filler", parser.parseFiller());

        Node::ptr rvalue =
            Variable::tryParseVariableValue<base::Box>(parser);
        if(!rvalue) {
            parser.logger()->log(Logger::ERROR,
                "A <box> was supposed to be here",
                parser, parser.lastToken());
            rvalue = Node::ptr(new Node("error_missing_box"));
            rvalue->setValue(Token::list());
        }

        node->appendChild("rvalue", rvalue);
        node->setValue(rvalue->valueAny());

        if(name.substr(0, 6) == "setbox")
            name = name.substr(3);
        parser.setSymbol(name, rvalue->valueAny(),
                    parser.isPrefixActive("\\global"));

        return true;
    }
    return false;
}

bool BoxSpec::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN || op == GET) {
        string name = parseName(parser, node);

        static vector<string> kw_spec;
        if(kw_spec.empty()) {
            kw_spec.push_back("to");
            kw_spec.push_back("spread");
        }

        Node::ptr spec = parser.parseOptionalKeyword(kw_spec);
        node->appendChild("spec_clause", spec);

        if(spec->value(string()) == "to") {
            node->appendChild("to", parser.parseDimen());
        } else if(spec->value(string()) == "spread") {
            node->appendChild("spread", parser.parseDimen());
        }

        node->appendChild("filler", parser.parseFiller());

        Parser::Mode prevMode = parser.mode();

        parser.beginGroup();
        parser.setMode(m_mode);
        parser.resetParagraphIndent();
        Node::ptr group = parser.parseGroup(Parser::GROUP_NORMAL);
        parser.setMode(prevMode);
        parser.endGroup();

        node->appendChild("content", group);


        node->setValue(group);
        return true;

    }
    return false;
}

} // namespace base
} // namespace texpp


