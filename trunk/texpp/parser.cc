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
#include <texpp/base/dimen.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <cassert>
#include <iterator>

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

void Parser::pushBack(vector< Token::ptr >* tokens)
{
    std::copy(tokens->rbegin(), tokens->rend(),
            std::front_inserter(m_tokenQueue));
    m_token.reset();
    // NOTE: lastToken is NOT changed
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

Node::ptr Parser::parseKeyword(const vector<string>& keywords)
{
    Node::ptr node(new Node("keyword"));

    while(helperIsImplicitCharacter(Token::CC_SPACE))
        nextToken(&node->tokens());

    string value;
    vector<string>::const_iterator kwEnd = keywords.end();
    for(size_t n=1; peekToken() && peekToken()->isCharacter(); ++n) {
        value += std::tolower(peekToken()->value()[0]);
        nextToken(&node->tokens());

        vector<string>::const_iterator kw = keywords.begin();
        for(; kw != kwEnd; ++kw) {
            if(kw->substr(0, n) == value) break;
        }

        if(kw == kwEnd) {
            pushBack(&node->tokens());
            break;
        } else if(kw->size() == n) {
            node->setValue(value);
            return node;
        }
    }

    return Node::ptr();
}

Node::ptr Parser::parseOptionalKeyword(const vector<string>& keywords)
{
    Node::ptr node = parseKeyword(keywords);
    if(!node) {
        node = Node::ptr(new Node("keyword"));
        while(helperIsImplicitCharacter(Token::CC_SPACE))
            nextToken(&node->tokens());
        node->setValue(string());
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
            node->setValue(int(48)); // XXX: why 48 ?
        }
        if(helperIsImplicitCharacter(Token::CC_SPACE))
            nextToken(&node->tokens());

        return node;
    }
    
    int result = 0;
    int digits = 0;

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
            if(result != TEXPP_INT_INV) {
                int v = isdigit(peekToken()->value()[0]) ?
                            peekToken()->value()[0]-'0' :
                            peekToken()->value()[0]-'A'+10;
                if(result<=TEXPP_INT_MAX/16 && result*16<=TEXPP_INT_MAX-v) {
                    result = result*16 + v;
                } else {
                    logger()->log(Logger::ERROR, "Number too big",
                                        *this, peekToken());
                    result = TEXPP_INT_INV;
                }
            }
            nextToken(&node->tokens()); ++digits;
        }

    } else if(peekToken()->isCharacter('\'', Token::CC_OTHER)) {
        nextToken(&node->tokens());
        while(peekToken() && 
                peekToken()->isCharacterCat(Token::CC_OTHER) &&
                        std::isdigit(peekToken()->value()[0]) &&
                peekToken()->value()[0] < '8') {
            if(result != TEXPP_INT_INV) {
                int v = peekToken()->value()[0] - '0';
                if(result<=TEXPP_INT_MAX/8 && result*8<=TEXPP_INT_MAX-v) {
                    result = result*8 + v;
                } else {
                    logger()->log(Logger::ERROR, "Number too big",
                                        *this, peekToken());
                    result = TEXPP_INT_INV;
                }
            }
            nextToken(&node->tokens()); ++digits;
        }

    } else {
        while(peekToken() && 
                peekToken()->isCharacterCat(Token::CC_OTHER) &&
                        std::isdigit(peekToken()->value()[0])) {
            if(result != TEXPP_INT_INV) {
                int v = peekToken()->value()[0] - '0';
                if(result<=TEXPP_INT_MAX/10 && result*10<=TEXPP_INT_MAX-v) {
                    result = result*10 + v;
                } else {
                    logger()->log(Logger::ERROR, "Number too big",
                                        *this, peekToken());
                    result = TEXPP_INT_INV;
                }
            }
            nextToken(&node->tokens()); ++digits;
        }
    }

    if(result == TEXPP_INT_INV)
        result = TEXPP_INT_MAX;

    if(!digits) {
        logger()->log(Logger::ERROR,
            "Missing number, treated as zero",
                                        *this, peekToken());
    }

    node->setValue(result);

    if(helperIsImplicitCharacter(Token::CC_SPACE))
        nextToken(&node->tokens());

    return node;
}

Node::ptr Parser::parseNormalDimen()
{
    Node::ptr node(new Node("normal_dimen"));
    if(!peekToken()) {
        logger()->log(Logger::ERROR,
            "Missing number, treated as zero", *this, peekToken());
        node->setValue(int(0));
        return node;
    }

    shared_ptr<base::InternalDimen> dimen = 
        parseCommandOrGroup<base::InternalDimen>(node);
    if(dimen) {
        node->setValue(dimen->getAny(*this));
        return node;
    }

    // Factor
    Node::ptr factor = parseDimenFactor();
    node->appendChild("factor", factor);
    pair<int, int> val = factor->value(std::make_pair(int(0), int(0)));
    bool overflow = false;

    // <optional_spaces>
    if(helperIsImplicitCharacter(Token::CC_SPACE))
        nextToken(&factor->tokens());

    // <internal unit>
    Node::ptr i_node(new Node("unternal_unit"));
    int i_unit = 0;

    shared_ptr<base::InternalInteger> i_int = 
        parseCommandOrGroup<base::InternalInteger>(i_node);
    if(i_int) {
        node->appendChild("internal_unit", i_node);
        i_unit = i_int->get(*this, int(0));

    } else {
        static vector<string> kw_internal_units;
        if(kw_internal_units.empty()) {
            kw_internal_units.push_back("em");
            kw_internal_units.push_back("ex");
        }

        i_node = parseKeyword(kw_internal_units);
        if(i_node) {
            node->appendChild("internal_unit", i_node);
            i_unit = 0; // TODO: fontdimen
        }
    }

    if(i_int || i_node) {
        if(i_unit != 0) {
            int v = TEXPP_SCALED_MAX;
            tuple<int,int,int> p = base::InternalDimen::multiplyIntFrac(
                        i_unit, val.second, 0x10000);
            if(i_unit < 0) { i_unit=-i_unit; val.first=-val.first; }
            if(!p.get<2>() && val.first <= TEXPP_SCALED_MAX/i_unit &&
                        val.first*i_unit <= TEXPP_SCALED_MAX-p.get<0>()) {
                v = val.first*i_unit + p.get<0>();
            } else {
                logger()->log(Logger::ERROR,
                    "Dimension too large", *this, lastToken());
                overflow = true;
                v = TEXPP_SCALED_MAX;
            }
            node->setValue(v);
        } else {
            node->setValue(int(0));
        }
        return node;
    }

    // <optional true>
    static vector<string> kw_optional_true;
    if(kw_optional_true.empty()) {
        kw_optional_true.push_back("true");
    }

    Node::ptr optional_true = parseKeyword(kw_optional_true);
    if(optional_true) {
        node->appendChild("optional_true", optional_true);
        int mag = symbol("mag", int(0));
        if(mag != 1000) {
            tuple<int, int, bool> p =
                base::InternalDimen::multiplyIntFrac(val.first, 1000, mag);
            if(!p.get<2>()) {
                val.first = p.get<0>();
                val.second = (1000*val.second + 0x10000*p.get<1>()) / mag;
                val.first = val.first + (val.second / 0x10000);
                val.second = val.second % 0x10000;
            } else {
                overflow = true;
            }
        }
    }

    // <physical units>
    static int u_scale[][2] = {
        {1,1},          // pt
        {1,1},          // sp
        {7227,100},     // in
        {12,1},         // pc
        {7227,254},     // cm
        {7227,2540},    // mm
        {7227,7200},    // bp
        {1238,1157},    // dd
        {14856,1157},   // cc
    };
    static vector<string> kw_physical_units;
    if(kw_physical_units.empty()) {
        kw_physical_units.push_back("pt");
        kw_physical_units.push_back("sp");
        kw_physical_units.push_back("in");
        kw_physical_units.push_back("pc");
        kw_physical_units.push_back("cm");
        kw_physical_units.push_back("mm");
        kw_physical_units.push_back("bp");
        kw_physical_units.push_back("dd");
        kw_physical_units.push_back("cc");
    }

    Node::ptr units = parseKeyword(kw_physical_units);
    if(units) {
        node->appendChild("physical_units", units);
        vector<string>::iterator it = std::find(kw_physical_units.begin(),
                    kw_physical_units.end(), units->value(string()));
        assert(it != kw_physical_units.end());
        int n = it - kw_physical_units.begin();
        if(n == 0) {
            // do nothing
        } else if(n == 1) { // sp
            val.second = val.first % 0x10000;
            val.first  = val.first / 0x10000;
        } else { // not pt
            tuple<int,int,bool> p = base::InternalDimen::multiplyIntFrac(
                        val.first,u_scale[n][0],u_scale[n][1]);
            overflow = p.get<2>();
            val.first = p.get<0>();
            val.second = (val.second * u_scale[n][0] +
                        p.get<1>() * 0x10000) / u_scale[n][1];
            val.first = val.first + (val.second / 0x10000);
            val.second = val.second % 0x10000;
        }
    } else {
        logger()->log(Logger::ERROR,
            "Illegal unit of measure (pt inserted)", *this, lastToken());
    }

    int v = val.first * 0x10000 + val.second;
    if(overflow || v >= 0x40000000) {
        logger()->log(Logger::ERROR,
            "Dimension too large", *this, lastToken());
        v = TEXPP_SCALED_MAX;
    }

    node->setValue(v);

    return node;
}

Node::ptr Parser::parseDimenFactor()
{
    if(peekToken() && peekToken()->isCharacterCat(Token::CC_OTHER) &&
            (std::isdigit(peekToken()->value()[0]) ||
             peekToken()->value()[0] == '.' ||
             peekToken()->value()[0] == ',')) {

        Node::ptr node(new Node("decimal_constant"));

        int result = 0;
        int frac = 0;
        int digits = 0;

        while(peekToken() && 
                peekToken()->isCharacterCat(Token::CC_OTHER) &&
                        std::isdigit(peekToken()->value()[0])) {
            if(result != TEXPP_INT_INV) {
                int v = peekToken()->value()[0] - '0';
                if(result<=TEXPP_INT_MAX/10 && result*10<=TEXPP_INT_MAX-v) {
                    result = result*10 + v;
                } else {
                    logger()->log(Logger::ERROR, "Number too big",
                                        *this, peekToken());
                    result = TEXPP_INT_INV;
                }
            }
            nextToken(&node->tokens()); ++digits;
        }

        if(result == TEXPP_INT_INV)
            result = TEXPP_INT_MAX;

        if(peekToken() && (peekToken()->isCharacter('.', Token::CC_OTHER) ||
                           peekToken()->isCharacter(',', Token::CC_OTHER))) {
            nextToken(&node->tokens()); ++digits;
            
            int fracDigits = 0;
            while(peekToken() && 
                    peekToken()->isCharacterCat(Token::CC_OTHER) &&
                            std::isdigit(peekToken()->value()[0])) {
                if(fracDigits < 17)
                    frac = (frac + (peekToken()->value()[0]-'0')*0x20000)/10;
                nextToken(&node->tokens()); ++fracDigits;
            }
            frac = (frac+1)/2;
        }

        if(!digits) {
            logger()->log(Logger::ERROR,
                "Missing number, treated as zero",
                                            *this, peekToken());
        }
        
        node->setValue(std::make_pair(int(result), frac));
        return node;

    } else {
        Node::ptr node = parseNormalInteger();
        node->setValue(std::make_pair(node->value(int(0)), 0));
        return node;
    }
}

Node::ptr Parser::parseNumber()
{
    Node::ptr node(new Node("number"));
    node->appendChild("sign", parseOptionalSigns());

    Node::ptr internal(new Node("internal"));
    shared_ptr<base::InternalDimen> dimen = 
        parseCommandOrGroup<base::InternalDimen>(internal);
    if(dimen) {
        internal->setType("internal_dimen");
        node->appendChild("coerced_dimen", internal);
        node->setValue(dimen->getAny(*this));
        return node;
    }

    node->appendChild("normal_integer", parseNormalInteger());

    node->setValue(node->child(0)->value(int(0)) *
                        node->child(1)->value(int(0)));
    return node;
}

Node::ptr Parser::parseDimen()
{
    Node::ptr node(new Node("dimen"));
    node->appendChild("sign", parseOptionalSigns());
    node->appendChild("normal_dimen", parseNormalDimen());

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

