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

bool Prefix::invokeWithPrefixes(Parser&, shared_ptr<Node>,
                                std::set<string>& prefixes)
{
    prefixes.insert(name());
    return true;
}

bool Assignment::invoke(Parser& parser, shared_ptr<Node> node)
{
    std::set<string> prefixes;
    return invokeWithPrefixes(parser, node, prefixes);
}

bool Assignment::checkPrefixes(Parser& parser,
        std::set<string> prefixes, bool macro)
{
    size_t ok = prefixes.count("\\global");
    bool global = ok;
    if(macro) {
        ok += prefixes.count("\\outer");
        ok += prefixes.count("\\long");
    }

    if(prefixes.size() != ok) {
        char escape = parser.symbol("escapechar", int('\\'));
        if(macro) {
            parser.logger()->log(Logger::ERROR,
                "You can't use such a prefix with `" + texRepr(&parser) + "'",
                parser, parser.lastToken());
        } else {
            parser.logger()->log(Logger::ERROR,
                string("You can't use `") + escape + "long' or `" +
                escape + "outer' with `" + texRepr(&parser) + "'",
                parser, parser.lastToken());
        }
    }

    int globaldefs = parser.symbol("globaldefs", int(0));
    if(globaldefs > 0) global = true;
    else if(globaldefs < 0) global = false;

    return global;
}

bool Let::invokeWithPrefixes(Parser& parser, shared_ptr<Node> node,
                                std::set<string>& prefixes)
{
    bool global = checkPrefixes(parser, prefixes);
    prefixes.clear();

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
            ltoken, parser.symbol(rtoken, Command::ptr()), global);
    } else {
        parser.setSymbol(
            ltoken, Command::ptr(new TokenCommand(rtoken)), global);
    }

    return true;
}

bool Futurelet::invokeWithPrefixes(Parser& parser, Node::ptr node,
                                    std::set<string>& prefixes)
{
    bool global = checkPrefixes(parser, prefixes);
    prefixes.clear();

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
            ltoken, parser.symbol(rtoken, Command::ptr()), global);
    } else {
        parser.setSymbol(
            ltoken, Command::ptr(new TokenCommand(rtoken)), global);
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

bool Def::invokeWithPrefixes(Parser& parser, shared_ptr<Node> node,
                                std::set<string>& prefixes)
{
    bool global = checkPrefixes(parser, prefixes, true);
    bool longAttr = prefixes.count("\\long");
    bool outerAttr = prefixes.count("\\outer");
    prefixes.clear();

    Node::ptr lvalue = parser.parseControlSequence(false);
    node->appendChild("lvalue", lvalue);

    Node::ptr paramsNode(new Node("params"));
    node->appendChild("params", paramsNode);

    int paramNum = 0;
    Token::list_ptr params(new Token::list);
    while(parser.peekToken(false)) {
        if(parser.peekToken(false)->isCharacterCat(Token::CC_BGROUP)) {
            break;
        } else if(parser.peekToken(false)->isCharacterCat(Token::CC_EGROUP)) {
            break;
        } else if(parser.peekToken(false)->isCharacterCat(Token::CC_PARAM)) {
            ++paramNum;
            int curParamNum = -1;
            params->push_back(
                parser.nextToken(&paramsNode->tokens(), false));
            if(parser.peekToken(false) &&
                    parser.peekToken(false)->isCharacter()) {
                char ch = parser.peekToken(false)->value()[0];
                if(std::isdigit(ch)) curParamNum = ch - '0';
            }

            if(paramNum > 9) {
                parser.logger()->log(Logger::ERROR,
                    "You already have nine parameters",
                    parser, parser.lastToken());
                params->pop_back();
            } else if(curParamNum != paramNum) {
                parser.logger()->log(Logger::ERROR,
                    "Parameters must be numbered consecutively",
                    parser, parser.lastToken());
                params->push_back(Token::ptr(
                    new Token(Token::TOK_CHARACTER, Token::CC_OTHER,
                                boost::lexical_cast<string>(paramNum))));
            } else {
                params->push_back(
                    parser.nextToken(&paramsNode->tokens(), false));
            }

        } else {
            params->push_back(parser.nextToken(&paramsNode->tokens(), false));
        }
    }

    paramsNode->setValue(params);

    Node::ptr left_brace(new Node("left_brace"));
    node->appendChild("left_brace", left_brace);

    if(parser.peekToken(false) && 
            parser.peekToken(false)->isCharacterCat(Token::CC_BGROUP)) {
        left_brace->setValue(parser.nextToken(&left_brace->tokens(), false));
    } else {
        parser.logger()->log(Logger::ERROR, "Missing { inserted",
            parser, parser.lastToken());
        left_brace->setValue(Token::ptr(new Token(
                    Token::TOK_CHARACTER, Token::CC_BGROUP, "{")));
    }

    Node::ptr definition = parser.parseBalancedText();
    node->appendChild("definition", definition);

    Node::ptr right_brace(new Node("right_brace"));
    node->appendChild("right_brace", right_brace);
    if(parser.peekToken(false) &&
            parser.peekToken(false)->isCharacterCat(Token::CC_EGROUP)) {
        right_brace->setValue(parser.nextToken(&right_brace->tokens()));
    } else {
        // TODO: error
        right_brace->setValue(Token::ptr(new Token(
                    Token::TOK_CHARACTER, Token::CC_EGROUP, "}")));
    }

    Token::ptr ltoken = lvalue->value(Token::ptr());
    parser.setSymbol(ltoken,
        Command::ptr(new UserMacro(ltoken ? ltoken->value() : "\\undefined",
            paramsNode->value(Token::list_ptr()),
            definition->value(Token::list_ptr()),
            outerAttr, longAttr)), global);

    return true;
}

string UserMacro::texRepr(Parser* parser) const
{
    string str;

    char escape = parser ? parser->symbol("escapechar", int(0)) : '\\';
    if(m_longAttr) str = str + escape + "long";
    if(m_outerAttr) str = str + escape + "outer";
    if(!str.empty()) str += ' ';

    str += "macro:\n";
    if(m_params) str += Token::texReprList(*m_params, parser);
    str += "->";
    if(m_definition) str += Token::texReprList(*m_definition, parser);

    return str;
}

} // namespace base
} // namespace texpp

