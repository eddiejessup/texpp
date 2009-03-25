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

#include <texpp/base/integer.h>
#include <texpp/parser.h>
#include <texpp/logger.h>
#include <texpp/lexer.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool InternalInteger::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN) {
        string name = parseName(parser, node);

        node->appendChild("equals", parser.parseOptionalEquals(false));
        Node::ptr rvalue = parser.parseNumber();
        node->appendChild("rvalue", rvalue);

        node->setValue(rvalue->valueAny());
        parser.setSymbol(name, rvalue->valueAny());
        return true;
    } else if(op == Variable::EXPAND) {
        string name = parseName(parser, node);
        int val = parser.symbol(name, int(0));
        node->setValue(boost::lexical_cast<string>(val));
        return true;
    } else {
        return Variable::invokeOperation(parser, node, op);
    }
}

bool IntegerVariable::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ADVANCE || op == MULTIPLY || op == DIVIDE) {
        string name = parseName(parser, node);

        static vector<string> kw_by(1, "by");
        node->appendChild("by", parser.parseOptionalKeyword(kw_by));

        Node::ptr rvalue = parser.parseNumber();
        node->appendChild("rvalue", rvalue);

        int v = parser.symbol(name, int(0));
        int rv = rvalue->value(int(0));
        bool overflow = false;

        if(op == ADVANCE) {
            v += rv;
        } else if(op == MULTIPLY) {
            pair<int,bool> p = safeMultiply(v, rv,  TEXPP_INT_MAX);
            v = p.first; overflow = p.second;
        } else if(op == DIVIDE) {
            pair<int,bool> p = safeDivide(v, rv);
            v = p.first; overflow = p.second;
        }

        if(overflow) {
            parser.logger()->log(Logger::ERROR,
                "Arithmetic overflow",
                parser, parser.lastToken());
        } else {
            node->setValue(v);
            parser.setSymbol(name, v);
        }
        return true;
    }

    return InternalInteger::invokeOperation(parser, node, op);
}

string CountRegister::parseName(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("variable_number", number);
    int n = number->value(int(0));

    if(n < 0 || n > 255) {
        parser.logger()->log(Logger::ERROR,
            "Bad register code (" + boost::lexical_cast<string>(n) + ")",
            parser, parser.lastToken());
        n = 0;
    }

    return name().substr(1) + boost::lexical_cast<string>(n);
}

string CharcodeVariable::parseName(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("variable_number", number);
    int n = number->value(int(0));

    if(n < 0 || n > 255) {
        parser.logger()->log(Logger::ERROR,
            "Bad character code (" + boost::lexical_cast<string>(n) + ")",
            parser, parser.lastToken());
        n = 0;
    }

    return name().substr(1) + boost::lexical_cast<string>(n);
}

bool CharcodeVariable::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN) {
        string name = parseName(parser, node);

        node->appendChild("equals", parser.parseOptionalEquals(false));
        Node::ptr rvalue = parser.parseNumber();
        node->appendChild("rvalue", rvalue);

        int n = rvalue->value(int(0));
        if(n < m_min || n > m_max) {
            parser.logger()->log(Logger::ERROR, "Invalid code (" +
                boost::lexical_cast<string>(n) +
                "), should be in the range " +
                boost::lexical_cast<string>(m_min) + ".." +
                boost::lexical_cast<string>(m_max),
                parser, parser.lastToken());
            n = 0;
        }

        node->setValue(n);
        parser.setSymbol(name, n);
        return true;
    } else {
        return InternalInteger::invokeOperation(parser, node, op);
    }
}

bool SpecialInteger::invokeOperation(Parser& parser,
                        shared_ptr<Node> node, Operation op)
{
    if(op == ASSIGN) {
        string name = parseName(parser, node);

        node->appendChild("equals", parser.parseOptionalEquals(false));
        Node::ptr rvalue = parser.parseNumber();
        node->appendChild("rvalue", rvalue);

        node->setValue(rvalue->valueAny());
        parser.setSymbol(name, rvalue->valueAny(), true); // global
        return true;
    } else {
        return InternalInteger::invokeOperation(parser, node, op);
    }
}

} // namespace base
} // namespace texpp

