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

#include <texpp/base/misc.h>
#include <texpp/base/parshape.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {
namespace base {

bool UnimplementedCommand::invoke(Parser& parser, shared_ptr<Node> node)
{
    parser.logger()->log(Logger::UNIMPLEMENTED,
        "Command " +
        node->child("control_token")->value(Token::ptr())->texRepr() +
        " is not yet implemented in TeXpp",
        parser, parser.lastToken());
    return true;
}

bool End::invoke(Parser& parser, shared_ptr<Node>)
{
    parser.end();
    return true;
}

bool Par::invoke(Parser& parser, shared_ptr<Node>)
{
    parser.resetParagraphIndent();
    if(parser.mode() == Parser::RHORIZONTAL)
        parser.setMode(Parser::RVERTICAL);
    else if(parser.mode() == Parser::HORIZONTAL)
        parser.setMode(Parser::VERTICAL);
    return true;
}

} // namespace base
} // namespace texpp

