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

#include <texpp/base/show.h>
#include <texpp/base/integer.h>
#include <texpp/base/dimen.h>
#include <texpp/base/glue.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool Show::parseArgs(Parser& parser, Node::ptr node)
{
    node->appendChild("token", parser.parseToken());
    return true;
}

bool Show::execute(Parser& parser, Node::ptr node)
{
    string str;
    Token::ptr token = node->child("token")->value(Token::ptr());
    if(token->isCharacter()) {
        str = token->meaning();
    } else {
        char escape = parser.symbol("escapechar", int(0));
        str = token->texRepr(escape) + '=';

        Command::ptr c = parser.symbol(token, Command::ptr());
        if(c) str += c->texRepr(escape);
        else str += "undefined";
    }
    parser.logger()->log(Logger::SHOW, str, parser, parser.lastToken());
    return true;
}

bool ShowThe::parseArgs(Parser& parser, Node::ptr node)
{
    Node::ptr arg(new Node("internal_quantity"));
    shared_ptr<InternalInteger> integer =
        parser.parseCommandOrGroup<InternalInteger>(arg);
    if(integer) {
        arg->setType("internal_integer");
        arg->setValue(integer->getAny(parser));
        node->appendChild("internal_quantity", arg);
        return true;
    }

    shared_ptr<InternalDimen> dimen =
        parser.parseCommandOrGroup<InternalDimen>(arg);
    if(dimen) {
        arg->setType("internal_dimen");
        arg->setValue(dimen->getAny(parser));
        node->appendChild("internal_quantity", arg);
        return true;
    }

    shared_ptr<InternalGlue> glue =
        parser.parseCommandOrGroup<InternalGlue>(arg);
    if(glue) {
        arg->setType("internal_glue");
        arg->setValue(glue->getAny(parser));
        node->appendChild("internal_quantity", arg);
        return true;
    }

    shared_ptr<InternalMuGlue> muglue =
        parser.parseCommandOrGroup<InternalMuGlue>(arg);
    if(muglue) {
        arg->setType("internal_muglue");
        arg->setValue(muglue->getAny(parser));
        node->appendChild("internal_quantity", arg);
        return true;
    }

    parser.logger()->log(Logger::ERROR,
        "You can't use `" + parser.peekToken()->texRepr() +
        "' after " + char(parser.symbol("escapechar", int(0))) + "the",
        parser, parser.lastToken());
    node->setValue(int(0));

    node->appendChild("internal_quantity", parser.parseToken());
    node->child("internal_quantity")->setValue(int(0));

    return true;
}

bool ShowThe::execute(Parser& parser, Node::ptr node)
{
    Node::ptr arg = node->child("internal_quantity");
    if(arg->type() == "internal_integer") {
        parser.logger()->log(Logger::SHOW,
            boost::lexical_cast<string>(arg->value(int(0))),
            parser, parser.lastToken());
    } else if(arg->type() == "internal_dimen") {
        parser.logger()->log(Logger::SHOW,
            InternalDimen::dimenToString(arg->value(int(0))),
            parser, parser.lastToken());
    } else if(arg->type() == "internal_glue") {
        parser.logger()->log(Logger::SHOW,
            InternalGlue::glueToString(arg->value(Glue(0))),
            parser, parser.lastToken());
    } else if(arg->type() == "internal_muglue") {
        parser.logger()->log(Logger::SHOW,
            InternalMuGlue::muGlueToString(arg->value(Glue(0))),
            parser, parser.lastToken());
    }
    return true;
}

} // namespace base
} // namespace texpp

