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

#include <texpp/parser.h>
#include <texpp/logger.h>

#include <texpp/base/base.h>
#include <texpp/base/variable.h>
#include <texpp/base/integer.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <cassert>

namespace texpp {

Node::ptr Node::child(const string& name)
{
    ChildrenList::iterator end = m_children.end();
    for(ChildrenList::iterator it = m_children.begin(); it != end; ++it) {
        if(it->first == name) return it->second;
    }
    return Node::ptr();
}

Token::ptr Node::lastToken()
{
    ChildrenList::reverse_iterator rend = m_children.rend();
    for(ChildrenList::reverse_iterator it = m_children.rbegin();
                                            it != rend; ++it) {
        Token::ptr token = it->second->lastToken();
        if(token) return token;
    }

    Token::list::reverse_iterator rend1 = m_tokens.rend();
    for(Token::list::reverse_iterator it1 = m_tokens.rbegin();
                                            it1 != rend1; ++it1) {
        if(!(*it1)->isSkipped()) return *it1;
    }

    return Token::ptr();
}

string Node::repr() const
{
    return "Node(" + reprString(m_type)
        + (m_value.empty() ? "" : ", " + reprAny(m_value))
        + ")";
}

string Node::treeRepr(size_t indent) const
{
    string str = repr();
    if(!m_children.empty()) {
        str += ":\n";
        ChildrenList::const_iterator end = m_children.end();
        for(ChildrenList::const_iterator it = m_children.begin();
                                            it != end; ++it) {
            str += string(indent+2, ' ') +
                    it->first + ": " + it->second->treeRepr(indent+2);
        }
    } else {
        str += '\n';
    }
    return str;
}

Parser::Parser(const string& fileName, std::istream* file,
                bool interactive, shared_ptr<Logger> logger)
    : m_logger(logger), m_groupLevel(0), m_end(false)
{
    if(!m_logger)
        m_logger = interactive ? shared_ptr<Logger>(new ConsoleLogger) :
                                 shared_ptr<Logger>(new NullLogger);
    m_lexer = shared_ptr<Lexer>(new Lexer(fileName, file, interactive, true));

    base::initSymbols(*this);
}

Parser::Parser(const string& fileName, std::auto_ptr<std::istream> file,
                bool interactive, shared_ptr<Logger> logger)
    : m_logger(logger), m_groupLevel(0), m_end(false)
{
    if(!m_logger)
        m_logger = interactive ? shared_ptr<Logger>(new ConsoleLogger) :
                                 shared_ptr<Logger>(new NullLogger);
    m_lexer = shared_ptr<Lexer>(new Lexer(fileName, file, interactive, true));

    base::initSymbols(*this);
}

any Parser::EMPTY_ANY;

const any& Parser::symbolAny(const string& name, bool global) const
{
    SymbolTable::const_iterator it = m_symbols.find(name);
    if(it != m_symbols.end()) {
        if(!global || it->second.first == 0) {
            return it->second.second;
        } else {
            it = m_symbolsGlobal.find(name);
            if(it != m_symbolsGlobal.end())
                return it->second.second;
        }
    }
    return EMPTY_ANY;
}

void Parser::setSymbol(const string& name, const any& value, bool global)
{
    SymbolTable::iterator it = m_symbols.find(name);
    if(it != m_symbols.end()) {
        if(!global) {
            if(it->second.first != m_groupLevel) {
                m_symbolsStack.push_back(std::make_pair(name, it->second));
                if(it->second.first == 0) m_symbolsGlobal[name] = it->second;
            }
            it->second.first = m_groupLevel;
            it->second.second = value;
        } else {
            if(it->second.first != 0) {
                m_symbolsGlobal[name] = std::make_pair(0, value);
            } else {
                it->second.second = value;
            }
        }
    } else {
        m_symbols[name] = std::make_pair(global ? 0 : m_groupLevel, value);
        if(!global && m_groupLevel != 0) {
            m_symbolsGlobal[name] = std::make_pair(0, any());
            m_symbolsStack.push_back(std::make_pair(name,
                                        std::make_pair(0, any())));
        }
    }
}

void Parser::beginGroup()
{
    m_symbolsStackLevels.push_back(m_symbolsStack.size());
    ++m_groupLevel;
}

void Parser::endGroup()
{
    assert(m_groupLevel > 0);
    size_t symbolsStackLevels = m_symbolsStackLevels.empty() ? 0 :
                                m_symbolsStackLevels.back();
    while(m_symbolsStack.size() > symbolsStackLevels) {
        SymbolStack::reference item = m_symbolsStack.back();
        SymbolTable::iterator it = m_symbols.find(item.first);

        if(item.second.first == 0) {
            SymbolTable::iterator it1 = m_symbolsGlobal.find(item.first);
            assert(it1 != m_symbolsGlobal.end());

            it->second = it1->second;
            m_symbolsGlobal.erase(it1);
            if(it->second.second.empty())
                m_symbols.erase(it);
        } else {
            it->second = item.second;
        }

        m_symbolsStack.pop_back();
    }
    m_symbolsStackLevels.pop_back();
    --m_groupLevel;
}

inline Token::ptr Parser::rawNextToken()
{
    if(!m_tokenQueue.empty()) {
        Token::ptr token = m_tokenQueue.front();
        m_tokenQueue.pop_front(); return token;
    } else {
        return m_lexer->nextToken();
    }
}

Token::ptr Parser::nextToken(vector< Token::ptr >* tokens)
{
    if(m_end) return Token::ptr();

    m_token.reset();

    // skip ignored tokens
    Token::ptr token = rawNextToken();
    while(token && token->isSkipped()) {
        if(token->catCode() == Token::CC_INVALID) {
            m_logger->log(Logger::ERROR,
                "Text line contains an invalid character", *this, token);
        }

        if(tokens) tokens->push_back(token);
        token = rawNextToken();
    }

    // real token
    if(tokens && token) tokens->push_back(token);
    Token::ptr ret = token;
    m_lastToken = token;

    // skip ignored tokens until EOL
    if(token && !token->isLastInLine()) {
        while(true) {
            token = rawNextToken();
            if(!token) {
                break;
            } else if(!token->isSkipped()) {
                //m_token = token;
                m_tokenQueue.push_front(token);
                break;
            }

            if(token->catCode() == Token::CC_INVALID) {
                m_logger->log(Logger::ERROR,
                    "Text line contains an invalid character", *this, token);
            }

            if(tokens) tokens->push_back(token);
            
            if(token->isLastInLine()) {
                break;
            }
        }
    }

    return ret;
}

Token::ptr Parser::lastToken()
{
    return m_lastToken;
}

Token::ptr Parser::peekToken()
{
    //int n = 1; // XXX
    if(m_end) return Token::ptr();

    if(m_token/* && n==1*/) return m_token; // cached token
   // m_token.reset();

    // check the queue
    std::deque<Token::ptr >::iterator end = m_tokenQueue.end();
    for(std::deque<Token::ptr >::iterator it = m_tokenQueue.begin();
                                                 it != end; ++it) {
        if(!(*it)->isSkipped()) {
            m_lastToken = m_token = *it;
            return m_token;
            /*if(!m_token) m_token = *it;
            if(!--n) return *it;*/
        }
    }

    // read tokens from input
    while(true) {
        Token::ptr token = m_lexer->nextToken();
        if(!token) break;

        m_tokenQueue.push_back(token);
        if(!token->isSkipped()) {
            m_lastToken = m_token = token;
            return m_token;
            /*if(!m_token) m_token = token;
            if(!--n) return token;*/
        }
    }

    return Token::ptr();
}

bool Parser::helperIsImplicitCharacter(Token::CatCode catCode)
{
    if(peekToken()) {
        if(peekToken()->isCharacterCat(catCode)) {
            return true;
        } else if(peekToken()->isControl()) {
            shared_ptr<TokenCommand> c =
                symbolCommand<TokenCommand>(peekToken());
            if(c && c->token()->isCharacterCat(catCode))
                return true;
        }
    }
    return false;
}

Node::ptr Parser::parseCommand(Command::ptr command)
{
    Node::ptr node(new Node("command"));
    node->appendChild("control_token", parseToken());
    command->parseArgs(*this, node); // XXX check errors
    return node;
}

Node::ptr Parser::parseToken()
{
    Node::ptr node(new Node("token"));
    Token::ptr token = peekToken();

    if(token) {
        node->setValue(nextToken(&node->tokens()));
    } else {
        logger()->log(Logger::ERROR, "Missing token inserted", *this, token);
        node->setValue(Token::ptr(new Token(Token::TOK_CONTROL,
                            Token::CC_ESCAPE, "inaccessible")));
    }

    return node;
}

Node::ptr Parser::parseControlSequence()
{
    Node::ptr node(new Node("control_sequence"));
    Token::ptr token = peekToken();

    if(token && token->isControl()) {
        node->setValue(nextToken(&node->tokens()));
    } else {
        logger()->log(Logger::ERROR,
            "Missing control sequence inserted", *this, token);
        node->setValue(Token::ptr(new Token(Token::TOK_CONTROL,
                            Token::CC_ESCAPE, "inaccessible")));
    }
    return node;
}

Node::ptr Parser::parseCharacter()
{
    Node::ptr node(new Node("character"));
    if(peekToken() && peekToken()->isCharacter()) {
        node->setValue(peekToken()->value());
        nextToken(&node->tokens());
    } else {
        logger()->log(Logger::ERROR,
            "Missing character inserted", *this, peekToken());
        node->setValue(string(""));
    }
    return node;
}

Node::ptr Parser::parseOptionalEquals(bool oneSpaceAfter)
{
    Node::ptr node(new Node("optional_equals"));
    while(helperIsImplicitCharacter(Token::CC_SPACE))
        nextToken(&node->tokens());

    if(peekToken() && peekToken()->isCharacter('=', Token::CC_OTHER)) {
        node->setValue(nextToken(&node->tokens()));
    }

    if(oneSpaceAfter && helperIsImplicitCharacter(Token::CC_SPACE))
        nextToken(&node->tokens());

    return node;
}

Node::ptr Parser::parseOptionalSigns()
{
    Node::ptr node(new Node("optional_signs"));
    node->setValue(int(1));

    while(peekToken() && (
                helperIsImplicitCharacter(Token::CC_SPACE) ||
                peekToken()->isCharacter('+', Token::CC_OTHER) ||
                peekToken()->isCharacter('-', Token::CC_OTHER))) {
        if(peekToken()->isCharacter('-', Token::CC_OTHER)) {
            node->setValue(- node->value(int(0)));
        }
        nextToken(&node->tokens());
    }

    return node;
}

Node::ptr Parser::tryParseInternalInteger()
{
    Command::ptr cmd = symbol(peekToken(), Command::ptr());
    if(dynamic_pointer_cast<base::InternalInteger>(cmd)) {
        Node::ptr node = parseToken();
        node->setType("internal_integer");
        node->setValue(
            static_pointer_cast<base::InternalInteger>(cmd)->getAny(*this));
        return node;
    } else if(dynamic_pointer_cast<CommandGroupBase>(cmd) &&
              dynamic_pointer_cast<base::InternalInteger>(
                static_pointer_cast<CommandGroupBase>(cmd)->item(0))) {
        Node::ptr node = parseToken();
        node->setType("internal_integer");
        Command::ptr cmd1 = static_pointer_cast<CommandGroupBase>(cmd)
                                ->parseCommand(*this, node);
        node->setValue(
            static_pointer_cast<base::InternalInteger>(cmd1)->getAny(*this));
        return node;
    } else {
        return Node::ptr();
    }
}

Node::ptr Parser::parseNormalInteger()
{

    Node::ptr node(new Node("normal_integer"));
    if(!peekToken()) {
        logger()->log(Logger::ERROR,
            "Missing number, treated as zero", *this, peekToken());
        node->setValue(int(0));

    }

    shared_ptr<base::InternalInteger> integer = 
        parseCommandOrGroup<base::InternalInteger>(node);
    if(integer) {
        node->setValue(integer->getAny(*this));
        return node;
    }

    if(peekToken()->isCharacter('`', Token::CC_OTHER)) {
        nextToken(&node->tokens());
        if(peekToken() && peekToken()->isCharacter()) {
            node->setValue(int(peekToken()->value()[0]));
            nextToken(&node->tokens());
        } else if(peekToken() && peekToken()->isControl() &&
                    peekToken()->value().size() == 2) {
            node->setValue(int(peekToken()->value()[1]));
            nextToken(&node->tokens());
        } else {
            logger()->log(Logger::ERROR,
                "Improper alphabetic constant", *this, peekToken());
            node->setValue(int(0));
        }
        if(helperIsImplicitCharacter(Token::CC_SPACE))
            nextToken(&node->tokens());

        return node;
    }
    
    std::stringstream digits;
    bool toobig = false;
    int result;

    if(peekToken()->isCharacter('\"', Token::CC_OTHER)) {
        nextToken(&node->tokens());

        while(peekToken() && ( 
                (peekToken()->isCharacterCat(Token::CC_OTHER) &&
                        std::isxdigit(peekToken()->value()[0]) &&
                        !std::islower(peekToken()->value()[0])) ||
                (peekToken()->isCharacterCat(Token::CC_LETTER) &&
                        std::isxdigit(peekToken()->value()[0]) &&
                        !std::islower(peekToken()->value()[0]) &&
                        !std::isdigit(peekToken()->value()[0])))) {
            digits << peekToken()->value()[0];
            digits.seekg(0, std::ios_base::beg);
            digits >> std::hex >> result;
            if(digits.fail()) {
                logger()->log(Logger::ERROR, "Number too big",
                                    *this, peekToken());
                toobig = true;
            }
            digits.clear();
            nextToken(&node->tokens());
        }

    } else if(peekToken()->isCharacter('\'', Token::CC_OTHER)) {
        nextToken(&node->tokens());
        while(peekToken() && 
                peekToken()->isCharacterCat(Token::CC_OTHER) &&
                        std::isdigit(peekToken()->value()[0]) &&
                peekToken()->value()[0] < '8') {
            digits << peekToken()->value()[0];
            digits.seekg(0, std::ios_base::beg);
            digits >> std::oct >> result;
            if(digits.fail()) {
                logger()->log(Logger::ERROR, "Number too big",
                                        *this, peekToken());
                toobig = true;
            }
            digits.clear();
            nextToken(&node->tokens());
        }

    } else {
        while(peekToken() && 
                peekToken()->isCharacterCat(Token::CC_OTHER) &&
                        std::isdigit(peekToken()->value()[0])) {
            digits << peekToken()->value()[0];
            digits.seekg(0, std::ios_base::beg);
            digits >> result;
            if(digits.fail()) {
                logger()->log(Logger::ERROR, "Number too big",
                                        *this, peekToken());
                toobig = true;
            }
            digits.clear();
            nextToken(&node->tokens());
        }
    }

    if(digits.str().size() == 0) {
        logger()->log(Logger::ERROR,
            "Missing number, treated as zero",
                                        *this, peekToken());
        result = 0;
    } else if(toobig) {
        result = INT_MAX;
    }

    node->setValue(result);

    if(helperIsImplicitCharacter(Token::CC_SPACE))
        nextToken(&node->tokens());

    return node;
}

Node::ptr Parser::parseNumber()
{
    Node::ptr node(new Node("number"));

    node->appendChild("sign", parseOptionalSigns());

    Command::ptr c = symbol(peekToken(), Command::ptr());

    /*
    shared_ptr<DimenVariable> dimen =
        dynamic_pointer_cast< shared_ptr<DimenVariable> >(c);
    if(dimen) {
        
    }

    shared_ptr<base::InternalInteger> integer = 
        dynamic_pointer_cast<base::InternalInteger>(c);
    if(integer) {
        Node::ptr child(new Node("internal_integer"));
        node->appendChild("unsigned_integer", child);
        child->setValue(integer->get(*this));
        nextToken(&child->tokens());
        return node;
    }*/

    node->appendChild("normal_integer", parseNormalInteger());
    node->setValue(node->child(0)->value(int(0)) *
                        node->child(1)->value(int(0)));
    return node;
}

Node::ptr Parser::parseBalancedText()
{
    Node::ptr node(new Node("balanced_text"));
    Token::list_ptr tokens(new Token::list);

    int level = 0;
    while(peekToken()) {
        if(peekToken()->isCharacterCat(Token::CC_BGROUP)) {
            ++level;
        } else if(peekToken()->isCharacterCat(Token::CC_EGROUP)) {
            if(--level < 0) break;
        }
        tokens->push_back(nextToken(&node->tokens()));
    }
    node->setValue(tokens);
    return node;
}

Node::ptr Parser::parseGeneralText(Node::ptr node)
{
    if(!node) node = Node::ptr(new Node("general_text"));

    // parse filler
    Node::ptr filler(new Node("filler"));
    node->appendChild("filler", filler);
    while(peekToken()) {
        if(helperIsImplicitCharacter(Token::CC_SPACE)) {
            nextToken(&filler->tokens());
            continue;
        } else if(peekToken()->isControl()) {
            Command::ptr obj = symbol(peekToken(), Command::ptr());
            if(obj && obj->name() == "\\relax") {
                nextToken(&filler->tokens());
                continue;
            }
        }
        break;
    }

    // parse left_brace_i
    Node::ptr left_brace(new Node("left_brace_i"));
    node->appendChild("left_brace", left_brace);
    if(helperIsImplicitCharacter(Token::CC_BGROUP)) {
        left_brace->setValue(nextToken(&left_brace->tokens()));
    } else {
        logger()->log(Logger::ERROR, "Missing { inserted", *this,peekToken());
        left_brace->setValue(Token::ptr(new Token(
                    Token::TOK_CHARACTER, Token::CC_BGROUP, "{")));
    }

    node->appendChild("balanced_text", parseBalancedText());

    // parse right_brace
    Node::ptr right_brace(new Node("right_brace"));
    node->appendChild("right_brace", right_brace);
    if(peekToken() && peekToken()->isCharacterCat(Token::CC_EGROUP)) {
        right_brace->setValue(nextToken(&right_brace->tokens()));
    } else {
        // TODO: error
        right_brace->setValue(Token::ptr(new Token(
                    Token::TOK_CHARACTER, Token::CC_EGROUP, "}")));
    }

    return node;
}

Node::ptr Parser::parseTextWord()
{
    Node::ptr node(new Node("text_word"));
    string value;
    while(peekToken() && peekToken()->isCharacterCat(Token::CC_LETTER)) {
        value += peekToken()->value();
        nextToken(&node->tokens());
    }
    node->setValue(value);
    return node;
}

Node::ptr Parser::parseGroup(Command::ptr endCmd, bool parseBeginEnd)
{
    Node::ptr node(new Node("group"));
    if(parseBeginEnd) {
        node->appendChild("group_begin", parseToken());
    }

    while(true) {
        if(!peekToken()) {
            if(parseBeginEnd) {
                // TODO: report error
                Node::ptr group_end(new Node("group_end"));
                node->appendChild("group_end", parseToken());
            }
            break;
        }

        if(parseBeginEnd) {
            if((endCmd && symbol(peekToken(), Command::ptr()) == endCmd) ||
                   (!endCmd && helperIsImplicitCharacter(Token::CC_EGROUP))) {
                node->appendChild("group_end", parseToken());
                break;
            }
        }

        if(peekToken()->isControl()) {
            Command::ptr cmd = symbol(peekToken(), Command::ptr());
            if(cmd) {
                node->appendChild("control", parseCommand(cmd));
                executeCommand(cmd, node->child(
                                        node->childrenCount()-1));
            } else {
                m_logger->log(Logger::ERROR, "Undefined control sequence",
                                                *this, peekToken());
                node->appendChild("error_unknown_control",
                                                parseToken());
            }

        } else if(peekToken()->isCharacterCat(Token::CC_LETTER)) {
            node->appendChild("text_word", parseTextWord());

        } else if(peekToken()->isCharacterCat(Token::CC_SPACE)) {
            node->appendChild("space", parseCharacter());

        } else if(peekToken()->isCharacterCat(Token::CC_OTHER)) {
            node->appendChild("character", parseCharacter());

        } else if(peekToken()->isCharacterCat(Token::CC_BGROUP)) {
            beginGroup();
            node->appendChild("group", parseGroup());
            
        } else if(peekToken()->isCharacterCat(Token::CC_EGROUP)) {
            m_logger->log(Logger::ERROR, "Too many }'s",
                                            *this, peekToken());
            node->appendChild("error_extra_group_end",
                                            parseToken());

        } else {
            node->appendChild("other_token", parseToken());
        }
    }

    return node;
}

Node::ptr Parser::parse()
{
    Node::ptr document = parseGroup(Command::ptr(), false);
    document->setType("document");
    
    // Some skipped tokens may still exists even when
    // peekToken reports EOF. Lets add that tokens to the last node.
    Node::ptr node = document;
    while(node->childrenCount() > 0)
        node = node->child(node->childrenCount()-1);

    nextToken(&node->tokens());

    return document;
}

} // namespace texpp

