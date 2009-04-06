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

#include <texpp/base/miscmacros.h>
#include <texpp/parser.h>
#include <texpp/logger.h>
#include <texpp/lexer.h>

#include <texpp/base/misc.h>

#include <boost/lexical_cast.hpp>

namespace texpp {
namespace base {

bool NumberMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("number", number);

    node->setValue(stringToTokens(
        boost::lexical_cast<string>(number->value(int(0)))));

    return true;
}

bool RomannumeralMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr number = parser.parseNumber();
    node->appendChild("number", number);

    static int BASIC_VALUES[] = { 1000, 900, 500, 400, 100, 90,
             50, 40, 10, 9, 5, 4, 1 };
    static const char* BASIC_ROMAN_NUMBERS[] = { "m", "cm", "d", "cd",
             "c", "xc", "l", "xl", "x", "ix", "v", "iv", "i" };

    string str;
    int num = number->value(int(0));
    for(size_t i = 0; i < sizeof(BASIC_VALUES)/sizeof(int); ++i) {
        while(num >= BASIC_VALUES[i]) {
            str += BASIC_ROMAN_NUMBERS[i];
            num -= BASIC_VALUES[i];
        }
    }

    node->setValue(stringToTokens(str));

    return true;
}

bool StringMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr tokenNode = parser.parseToken(false);
    Token::ptr token = tokenNode->value(Token::ptr());
    node->appendChild("token", tokenNode);

    string str;
    if(token->isControl()) {
        str = token->value();
    } else {
        str = token->texRepr(parser.symbol("escapechar", int(0)));
    }

    node->setValue(stringToTokens(str));
    return true;
}

bool CsnameMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr child(new Node("args"));
    node->appendChild("args", child);

    string name(1, '\\');
    while(parser.peekToken() && parser.peekToken()->isCharacter()) {
        name += parser.peekToken()->value();
        parser.nextToken(&child->tokens());
    }

    Command::ptr cmd =
        parser.symbolCommand<EndcsnameMacro>(parser.peekToken());
    if(cmd) {
        parser.nextToken(&child->tokens());
    } else {
        char escape = parser.symbol("escapechar", int(0));
        parser.logger()->log(Logger::ERROR,
            "Missing " + string(1, escape) + "endcsname inserted",
            parser, parser.lastToken());
    }

    if(!parser.symbol(name, Command::ptr())) {
        // TODO: Do not create new "\\relax", use exising one instead!
        parser.setSymbol(name, Command::ptr(new Relax("\\relax")));
    }

    node->setValue(Token::list(1, Token::ptr(
        new Token(Token::TOK_CONTROL, Token::CC_ESCAPE, name))));

    return true;
}

bool EndcsnameMacro::invoke(Parser& parser, shared_ptr<Node>)
{
    char escape = parser.symbol("escapechar", int(0));
    parser.logger()->log(Logger::ERROR,
        "Extra " + texRepr(escape),
        parser, parser.lastToken());

    return true;
}

bool ExpandafterMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr child1 = parser.parseToken(false); // don't expand
    node->appendChild("token1", child1);

    Node::ptr child2 = parser.parseToken(true); // expand
    node->appendChild("token2", child2);

    Token::ptr token1 = child1->value(Token::ptr());
    Token::ptr token2 = child2->value(Token::ptr());
    Token::list tokens;
    if(token1)
        tokens.push_back(Token::ptr(new Token(token1->type(),
                        token1->catCode(), token1->value())));
    if(token2)
        tokens.push_back(Token::ptr(new Token(token2->type(),
                        token2->catCode(), token2->value())));

    parser.pushBack(&tokens);

    return true;
}

bool NoexpandMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr child = parser.parseToken(false);
    node->appendChild("token", child);

    Token::ptr token = child->value(Token::ptr());
    parser.setNoexpand(token);
    node->setValue(Token::list(1, token));

    return true;
}

bool JobnameMacro::expand(Parser& parser, shared_ptr<Node> node)
{
    node->setValue(stringToTokens(parser.lexer()->jobName()));
    return true;
}

} // namespace base
} // namespace texpp


