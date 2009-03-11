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

#include <texpp/command.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

namespace texpp {

string Command::texRepr(char escape) const
{
    if(!m_name.empty() && m_name[0] == '\\') {
        string ret = m_name;
        ret[0] = escape;
        return ret;
    }
    return m_name;
}

string Command::repr() const
{
    return "Command(" + reprString(name())
            + ", " + reprString(texRepr()) + ")";
}

string TokenCommand::texRepr(char) const
{
    return m_token->meaning();
}

bool TokenCommand::parseArgs(Parser&, Node::ptr node)
{
    node->setValue(m_token);
    return true;
}

bool TokenCommand::execute(Parser&, Node::ptr)
{
    // XXX: TODO
    return true;
}

Command::ptr CommandGroupBase::parseCommand(Parser& parser, Node::ptr node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("command_number", number);

    Command::ptr cmd = item(number->value(int(0)));
    if(!cmd) {
        parser.logger()->log(Logger::ERROR, "Bad " + groupType() +
           " code (" + boost::lexical_cast<string>(number->value(int(0))) +
           ")", parser, parser.lastToken());
        
        cmd = item(0);
    }

    return cmd;
}

bool CommandGroupBase::parseArgs(Parser& parser, Node::ptr node)
{
    Command::ptr cmd = parseCommand(parser, node);
    if(cmd) return cmd->parseArgs(parser, node);
    else return false;
}

bool CommandGroupBase::execute(Parser& parser, Node::ptr node)
{
    Command::ptr cmd = item(node->child("command_number")->value(int(0)));
    if(!cmd) cmd = item(0);
    if(cmd) return cmd->execute(parser, node);
    else return false;
}

} // namespace texpp

