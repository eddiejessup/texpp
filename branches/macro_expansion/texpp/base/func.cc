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

#include <texpp/base/func.h>
#include <texpp/parser.h>

namespace texpp {
namespace base {

bool Prefix::invoke(Parser& parser, shared_ptr<Node>)
{
    parser.activePrefixes().insert(name());
    return true;
}

bool Let::invoke(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr lvalue = parser.parseControlSequence(false);
    node->appendChild("lvalue", lvalue);

    Node::ptr equals(new Node("optional_equals"));
    node->appendChild("equals", equals);

    while(parser.helperIsImplicitCharacter(Token::CC_SPACE, false))
        parser.nextToken(&equals->tokens());

    if(parser.peekToken(false) && \
            parser.peekToken(false)->isCharacter('=', Token::CC_OTHER)) {
        equals->setValue(parser.nextToken(&equals->tokens()));
    }

    if(parser.helperIsImplicitCharacter(Token::CC_SPACE, false))
        parser.nextToken(&equals->tokens());

    Node::ptr rvalue = parser.parseToken(false);
    node->appendChild("rvalue", rvalue);

    Token::ptr ltoken = lvalue->value(Token::ptr());
    Token::ptr rtoken = rvalue->value(Token::ptr());

    if(rtoken->isControl()) {
        parser.setSymbol(
            ltoken, parser.symbol(rtoken, Command::ptr()),
            parser.isPrefixActive("\\global")
            );
    } else {
        parser.setSymbol(
            ltoken, Command::ptr(new TokenCommand(rtoken)),
            parser.isPrefixActive("\\global")
            );
    }

    return true;
}

bool Futurelet::invoke(Parser& parser, Node::ptr node)
{
    Node::ptr lvalue = parser.parseControlSequence(false);
    node->appendChild("lvalue", lvalue);

    Node::ptr tokenNode = parser.parseToken(false);
    node->appendChild("token", tokenNode);

    Node::ptr rvalue = parser.parseToken(false);
    node->appendChild("rvalue", rvalue);

    Token::ptr ltoken = lvalue->value(Token::ptr());
    Token::ptr rtoken = rvalue->value(Token::ptr());
    Token::ptr token = tokenNode->value(Token::ptr());

    if(rtoken->isControl()) {
        parser.setSymbol(
            ltoken, parser.symbol(rtoken, Command::ptr()),
            parser.isPrefixActive("\\global")
            );
    } else {
        parser.setSymbol(
            ltoken, Command::ptr(new TokenCommand(rtoken)),
            parser.isPrefixActive("\\global")
            );
    }

    Token::list tokens;
    if(token)
        tokens.push_back(Token::ptr(new Token(token->type(),
                        token->catCode(), token->value())));
    if(rtoken)
        tokens.push_back(Token::ptr(new Token(rtoken->type(),
                        rtoken->catCode(), rtoken->value())));

    parser.pushBack(&tokens);

    return true;
}

/*
bool FutureLet::execute(Parser& parser, Node::ptr node)
{
    parser.setSymbol(
        node->child(1)->value(Token::ptr()),
        parser.symbol(
            node->child(3)->value(Token::ptr()),
            Command::ptr()
        ),
        parser.isPrefixActive("\\global")
    );
    return true;
}
*/

} // namespace base
} // namespace texpp

