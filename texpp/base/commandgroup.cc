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

#include <texpp/base/commandgroup.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {
namespace base {

bool CommandGroupBase::parseArgs(Parser& parser, Node::ptr node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("command_number", number);

    Command::ptr cmd = item(number->value(int(0)));
    if(!cmd) {
        Node::ptr node1 = number;
        while(node1->childrenCount() > 0)
            node1 = node1->child(node1->childrenCount()-1);

        std::ostringstream msg;
        msg << "Bad register code (" << number->value(int(0)) << ")";
        parser.logger()->log(Logger::ERROR, msg.str(), parser,
            node1->tokens().size() > 0 ? node1->tokens().back() : Token::ptr());
        
        cmd = item(0);
    }

    if(cmd) return cmd->parseArgs(parser, node);
    else return false;
}

bool CommandGroupBase::execute(Parser& parser, Node::ptr node)
{
    Command::ptr cmd = item(node->child("command_number")->value(int(0)));
    if(cmd) return cmd->execute(parser, node);
    else return false;
}

} // namespace base
} // namespace texpp

