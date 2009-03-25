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

#include <texpp/base/glue.h>
#include <texpp/base/dimen.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <sstream>

namespace texpp {
namespace base {

namespace {
bool invokeGlueOperation(Variable& var, Parser& parser,
                shared_ptr<Node> node, Variable::Operation op, bool mu)
{
    if(op == Variable::ASSIGN) {
        string name = var.parseName(parser, node);

        node->appendChild("equals", parser.parseOptionalEquals(false));
        Node::ptr rvalue = parser.parseGlue(mu);
        node->appendChild("rvalue", rvalue);

        node->setValue(rvalue->valueAny());
        parser.setSymbol(name, rvalue->valueAny());
        return true;
    } else if(op == Variable::EXPAND) {
        string name = var.parseName(parser, node);
        Glue val = parser.symbol(name, Glue(0));
        node->setValue(InternalGlue::glueToString(val, mu));
        return true;
    } else {
        return var.Variable::invokeOperation(parser, node, op);
    }
}

bool invokeGlueVarOperation(Variable& var, Parser& parser,
                shared_ptr<Node> node, Variable::Operation op, bool mu)
{
    static vector<string> kw_by(1, "by");
    if(op == Variable::ADVANCE) {
        string name = var.parseName(parser, node);
        
        node->appendChild("by", parser.parseOptionalKeyword(kw_by));

        Node::ptr rvalue = parser.parseGlue(mu);
        node->appendChild("rvalue", rvalue);

        Glue v = parser.symbol(name, Glue(0));
        Glue rv = rvalue->value(Glue(0));

        v.width += rv.width;

        if(v.stretch == 0) v.stretchOrder = 0;
        if(v.stretchOrder == rv.stretchOrder) {
            v.stretch += rv.stretch;
        } else if(v.stretchOrder < rv.stretchOrder && rv.stretch != 0) {
            v.stretch = rv.stretch; v.stretchOrder = rv.stretchOrder;
        }

        if(v.shrink == 0) v.shrinkOrder = 0;
        if(v.shrinkOrder == rv.shrinkOrder) {
            v.shrink += rv.shrink;
        } else if(v.shrinkOrder < rv.shrinkOrder && rv.shrink != 0) {
            v.shrink = rv.shrink; v.shrinkOrder = rv.shrinkOrder;
        }

        node->setValue(v);
        parser.setSymbol(name, v);
        return true;

    } else if(op == Variable::MULTIPLY || op == Variable::DIVIDE) {
        string name = var.parseName(parser, node);

        node->appendChild("by", parser.parseOptionalKeyword(kw_by));

        Node::ptr rvalue = parser.parseNumber();
        node->appendChild("rvalue", rvalue);

        Glue v = parser.symbol(name, Glue(0));
        int rv = rvalue->value(int(0));
        bool overflow = false;

        if(op == Variable::MULTIPLY) {
            pair<int,bool> p = safeMultiply(v.width, rv, TEXPP_SCALED_MAX);
            v.width = p.first; overflow |= p.second;

            p = safeMultiply(v.stretch, rv, TEXPP_SCALED_MAX);
            v.stretch = p.first; overflow |= p.second;

            p = safeMultiply(v.shrink, rv, TEXPP_SCALED_MAX);
            v.shrink = p.first; overflow |= p.second;

        } else if(op == Variable::DIVIDE) {
            pair<int,bool> p = safeDivide(v.width, rv);
            v.width = p.first; overflow |= p.second;

            p = safeDivide(v.stretch, rv);
            v.stretch = p.first; overflow |= p.second;

            p = safeDivide(v.shrink, rv);
            v.shrink = p.first; overflow |= p.second;
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

    return invokeGlueOperation(var, parser, node, op, mu);
}
}

bool InternalGlue::invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op)
{
    return invokeGlueOperation(*this, parser, node, op, false);
}

bool GlueVariable::invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op)
{
    return invokeGlueVarOperation(*this, parser, node, op, false);
}

bool InternalMuGlue::invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op)
{
    return invokeGlueOperation(*this, parser, node, op, true);
}

bool MuGlueVariable::invokeOperation(Parser& parser,
                shared_ptr<Node> node, Operation op)
{
    return invokeGlueVarOperation(*this, parser, node, op, true);
}

string InternalGlue::glueToString(const Glue& g, bool mu)
{
    string s = InternalDimen::dimenToString(g.width, 0, mu);
    if(g.stretch) {
        s = s + " plus " +
            InternalDimen::dimenToString(g.stretch, g.stretchOrder, mu);
    }
    if(g.shrink) {
        s = s + " minus " +
            InternalDimen::dimenToString(g.shrink, g.shrinkOrder, mu);
    }
    return s;
}

} // namespace base
} // namespace texpp


