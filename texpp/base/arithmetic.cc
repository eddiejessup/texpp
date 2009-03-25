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

#include <texpp/base/arithmetic.h>
#include <texpp/base/integer.h>
#include <texpp/base/dimen.h>
#include <texpp/base/glue.h>
#include <texpp/parser.h>
#include <texpp/logger.h>
#include <texpp/lexer.h>

namespace texpp {
namespace base {

/*
bool ArithmeticCommand::parseArgs(Parser& parser, Node::ptr node)
{
    static vector<string> kw_by(1, "by");

    Node::ptr arg(new Node("lvalue"));
    shared_ptr<IntegerVariable> integer =
        parser.parseCommandOrGroup<IntegerVariable>(arg);
    if(integer) {
        arg->setType("integer_variable");
        arg->setValue(integer->name());
        node->appendChild("lvalue", arg);
        node->appendChild("optional_by", parser.parseOptionalKeyword(kw_by));
        node->appendChild("rvalue", parser.parseNumber());
        return true;
    }

    shared_ptr<DimenVariable> dimen =
        parser.parseCommandOrGroup<DimenVariable>(arg);
    if(dimen) {
        arg->setType("dimen_variable");
        arg->setValue(dimen->name());
        node->appendChild("lvalue", arg);
        node->appendChild("optional_by", parser.parseOptionalKeyword(kw_by));
        node->appendChild("rvalue", m_type == ADVANCE ?
                parser.parseDimen() : parser.parseNumber());
        return true;
    }

    shared_ptr<GlueVariable> glue =
        parser.parseCommandOrGroup<GlueVariable>(arg);
    if(glue) {
        arg->setType("glue_variable");
        arg->setValue(glue->name());
        node->appendChild("lvalue", arg);
        node->appendChild("optional_by", parser.parseOptionalKeyword(kw_by));
        node->appendChild("rvalue", m_type == ADVANCE ?
                parser.parseGlue() : parser.parseNumber());
        return true;
    }

    shared_ptr<MuGlueVariable> muglue =
        parser.parseCommandOrGroup<MuGlueVariable>(arg);
    if(muglue) {
        arg->setType("muglue_variable");
        arg->setValue(muglue->name());
        node->appendChild("lvalue", arg);
        node->appendChild("optional_by", parser.parseOptionalKeyword(kw_by));
        node->appendChild("rvalue", m_type == ADVANCE ?
                parser.parseGlue(true) : parser.parseNumber());
        return true;
    }

    parser.logger()->log(Logger::ERROR,
        string("You can't use `") + parser.peekToken()->texRepr() +
        string("' after ") + texRepr(),
        parser, parser.lastToken());
    node->setValue(int(0));

    node->appendChild("lvalue", Node::ptr(new Node("error_missing")));
    node->appendChild("rvalue", Node::ptr(new Node("error_missing")));

    return true;
}

bool ArithmeticCommand::execute(Parser& parser, Node::ptr node)
{
    Node::ptr arg = node->child("lvalue");
    enum { INTEGER, DIMEN, GLUE, MUGLUE } type;
    if(arg->type() == "integer_variable") type = INTEGER;
    else if(arg->type() == "dimen_variable") type = DIMEN;
    else if(arg->type() == "glue_variable") type = GLUE;
    else if(arg->type() == "muglue_variable") type = MUGLUE;

    if(type == INTEGER || type == DIMEN) {
        string name = arg->value(string());
        if(name.empty()) return true;
        name = name.substr(1);

        int v1 = parser.symbol(name, int(0));
        int v2 = node->child("rvalue")->value(int(0));
        if(m_type == ADVANCE) {
            v1 += v2;
        } else {
            bool overflow = false;
            if(m_type == MULTIPLY) {
                pair<int,bool> p = safeMultiply(v1, v2, 
                            type==INTEGER ? TEXPP_INT_MAX : TEXPP_SCALED_MAX);
                v1 = p.first;
                overflow = p.second;
            } else if(m_type == DIVIDE) {
                pair<int,bool> p = safeDivide(v1, v2);
                v1 = p.first;
                overflow = p.second;
            }
            if(overflow) {
                parser.logger()->log(Logger::ERROR,
                    "Arithmetic overflow",
                    parser, parser.lastToken());
                return true;
            }
        }

        parser.setSymbol(name, v1);
        if(name == "endlinechar") // XXX: better solution?
            parser.lexer()->setEndlinechar(v1);

    } else if(type == GLUE || type == MUGLUE) {
        string name = arg->value(string());
        if(name.empty()) return true;
        name = name.substr(1);

        Glue v1 = parser.symbol(name, Glue(0));
        bool overflow = false;

        if(m_type == ADVANCE) {
            Glue v2 = node->child("rvalue")->value(Glue(0));
            v1.width += v2.width;

            if(v1.stretch == 0) v1.stretchOrder = 0;
            if(v1.stretchOrder == v2.stretchOrder) {
                v1.stretch += v2.stretch;
            } else if(v1.stretchOrder < v2.stretchOrder && v2.stretch != 0) {
                v1.stretch = v2.stretch; v1.stretchOrder = v2.stretchOrder;
            }

            if(v1.shrink == 0) v1.shrinkOrder = 0;
            if(v1.shrinkOrder == v2.shrinkOrder) {
                v1.shrink += v2.shrink;
            } else if(v1.shrinkOrder < v2.shrinkOrder && v2.shrink != 0) {
                v1.shrink = v2.shrink; v1.shrinkOrder = v2.shrinkOrder;
            }
        } else if(m_type == MULTIPLY) {
            int v2 = node->child("rvalue")->value(int(0));
            pair<int, bool> p = safeMultiply(
                        v1.width, v2, TEXPP_SCALED_MAX);
            v1.width = p.first; overflow |= p.second;

            p = safeMultiply(v1.stretch, v2, TEXPP_SCALED_MAX);
            v1.stretch = p.first; overflow |= p.second;

            p = safeMultiply(v1.shrink, v2, TEXPP_SCALED_MAX);
            v1.shrink = p.first; overflow |= p.second;
        } else if(m_type == DIVIDE) {
            int v2 = node->child("rvalue")->value(int(0));
            pair<int, bool> p = safeDivide(v1.width, v2);
            v1.width = p.first; overflow |= p.second;

            p = safeDivide(v1.stretch, v2);
            v1.stretch = p.first; overflow |= p.second;

            p = safeDivide(v1.shrink, v2);
            v1.shrink = p.first; overflow |= p.second;
        }
        if(overflow) {
            parser.logger()->log(Logger::ERROR,
                "Arithmetic overflow",
                parser, parser.lastToken());
            return true;
        }
        parser.setSymbol(name, v1);
    }

    return true;
}
*/

} // namespace base
} // namespace texpp

