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

bool IntegerVariable::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("equals", parser.parseOptionalEquals(false));
    node->appendChild("rvalue", parser.parseNumber());
    return check(parser, node->child("rvalue"));
}

bool IntegerVariable::execute(Parser& parser, Node::ptr node)
{
    return set(parser, node->child("rvalue")->value(int(0)));
}

bool EndlinecharVariable::set(Parser& parser, const any& value, bool global)
{
    if(IntegerVariable::set(parser, value, global)) {
        assert(value.type() == typeid(int));
        parser.lexer()->setEndlinechar(*unsafe_any_cast<int>(&value));
        return true;
    } else {
        return false;
    }
}

bool CharcodeVariable::check(Parser& parser, Node::ptr node)
{
    assert(node->valueAny().type() == typeid(int));
    int n = node->value(int(0));
    if(n < 0 || n > 255) {
        parser.logger()->log(Logger::ERROR, "Invalid code (" +
                                boost::lexical_cast<string>(n) +
                                "), should be in the range 0..255",
                                parser, node->lastToken());

        return false;
    }
    return true;
}

bool CatcodeVariable::check(Parser& parser, Node::ptr node)
{
    assert(node->valueAny().type() == typeid(int));
    int n = node->value(int(0));
    if(n < 0 || n > 15) {
        parser.logger()->log(Logger::ERROR, "Invalid code (" +
                                boost::lexical_cast<string>(n) +
                                "), should be in the range 0..15",
                                parser, node->lastToken());

        node->setValue(int(0));
    }
    return true;
}

bool CatcodeVariable::set(Parser& parser, const any& value, bool global)
{
    if(CharcodeVariable::set(parser, value, global)) {
        assert(value.type() == typeid(int));
        assert(name().substr(0, 8) == "\\catcode");
        std::istringstream str(name().substr(8));
        int n = boost::lexical_cast<int>(name().substr(8));
        assert(n >= 0 && n < 256);
        parser.lexer()->setCatcode(n, *unsafe_any_cast<int>(&value));
        return true;
    } else {
        return false;
    }
}

} // namespace base
} // namespace texpp

