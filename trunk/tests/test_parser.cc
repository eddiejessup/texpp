/* This file is part of texpp.
   Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

   texpp is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   texpp is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with texpp.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BOOST_TEST_MODULE parser_test_suite
#include <boost/test/included/unit_test.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <texpp/parser.h>
#include <texpp/logger.h>
#include <iostream>
#include <sstream>

using namespace texpp;

class TestLogger: public Logger
{
public:
    bool log(int, const string& message,
                Parser&, shared_ptr<Token> token) {
        logMessages.push_back(message);
        logPositions.push_back(
            std::make_pair(token->lineNo(), token->charPos()));
        return true;
    }
    vector<string> logMessages;
    vector<std::pair<size_t, size_t> > logPositions;
};

shared_ptr<Parser> create_parser(const string& input)
{
    std::auto_ptr<std::istream> ifile(new std::istringstream(input));
    return shared_ptr<Parser>(
        new Parser("", ifile, false, shared_ptr<Logger>(new TestLogger)));
}

BOOST_AUTO_TEST_CASE( parser_tokens )
{
    shared_ptr<Parser> parser = create_parser("  a %  \n  \\bb\\ \\c");
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->peekToken(1)->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->peekToken(2)->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 1, 3, 4).repr());
    BOOST_CHECK_EQUAL(parser->peekToken(3)->repr(),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 2, 2, 5).repr());
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 1, 2, 3).repr());

    Token tokens[] = {
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 1, 2, 3),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 1, 3, 4),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 2, 2, 5),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\ ", "\\ ", 2, 5, 7),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\c", "\\c", 2, 7, 9),
    };

    Token tokens_all[][5] = {
        {
            Token(Token::TOK_SKIPPED, Token::CC_SPACE, " ", "  ", 1, 0, 2),
            Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 1, 2, 3)
        },{
            Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 1, 3, 4),
            Token(Token::TOK_SKIPPED, Token::CC_COMMENT, "%", "%  \n", 1, 4, 8)
        },{
            Token(Token::TOK_SKIPPED, Token::CC_SPACE, " ", "  ", 2, 0, 2),
            Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 2, 2, 5)
        },{
            Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\ ", "\\ ", 2, 5, 7)
        },{
            Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\c", "\\c", 2, 7, 9)
        },
    };
    int tokens_all_counts[] = {2,2,2,1,1};

    vector<string> output_repr;
    Token::ptr token, token1, token2;

    size_t n = 0;
    while(token = parser->peekToken()) {
        vector<Token::ptr> output_all;

        token1 = parser->peekToken(2);
        token2 = parser->nextToken(&output_all);

        BOOST_CHECK_EQUAL(token->repr(), token2->repr());

        if(token1)
            BOOST_CHECK_EQUAL(token1->repr(), parser->peekToken()->repr());
        else
            BOOST_CHECK_EQUAL(Token::ptr(), parser->peekToken());

        if(n < sizeof(tokens)/sizeof(Token)) {
            vector<string> output_all_repr(output_all.size());
            vector<string> tokens_all_repr(tokens_all_counts[n]);

            std::transform(output_all.begin(), output_all.end(),
                    output_all_repr.begin(),
                    boost::lambda::bind(&Token::repr, *boost::lambda::_1));

            std::transform(tokens_all[n], tokens_all[n] + tokens_all_counts[n],
                    tokens_all_repr.begin(),
                    boost::lambda::bind(&Token::repr, boost::lambda::_1));

            BOOST_CHECK_EQUAL_COLLECTIONS(
                    tokens_all_repr.begin(), tokens_all_repr.end(),
                    output_all_repr.begin(), output_all_repr.end());
        }
        
        output_repr.push_back(token->repr());
        ++n;
    }
    
    vector<string> tokens_repr(sizeof(tokens)/sizeof(Token));
    std::transform(tokens, tokens + tokens_repr.size(), tokens_repr.begin(),
                    boost::lambda::bind(&Token::repr, boost::lambda::_1));

    BOOST_CHECK_EQUAL_COLLECTIONS(tokens_repr.begin(), tokens_repr.end(),
                                  output_repr.begin(), output_repr.end());
}

BOOST_AUTO_TEST_CASE( parser_symbols )
{
    shared_ptr<Parser> parser = create_parser("");
    
    parser->setSymbol("a", 1);
    parser->setSymbol("b", string("2"));
    parser->setSymbol("c", double(2.2));

    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0));
    BOOST_CHECK_EQUAL(string("2"), parser->symbol("b", string()));
    BOOST_CHECK_EQUAL(double(2.2), parser->symbol("c", double(0.0)));

    BOOST_CHECK_EQUAL(string("x"), parser->symbol("c", string("x")));
    BOOST_CHECK_EQUAL(2, parser->symbol("b", 2));

    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0, false));
    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0, true));

    parser->beginGroup();

    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0, false));
    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0, true));
    
    parser->setSymbol("a", 2);
    BOOST_CHECK_EQUAL(2, parser->symbol("a", 0, false));
    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0, true));

    parser->setSymbol("a", 3, true);
    BOOST_CHECK_EQUAL(2, parser->symbol("a", 0, false));
    BOOST_CHECK_EQUAL(3, parser->symbol("a", 0, true));

    parser->setSymbol("d", 5);
    BOOST_CHECK_EQUAL(5, parser->symbol("d", 0, false));
    BOOST_CHECK_EQUAL(0, parser->symbol("d", 0, true));

    parser->setSymbol("e", 7, true);
    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0, false));
    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0, true));

    parser->setSymbol("e", 8);
    BOOST_CHECK_EQUAL(8, parser->symbol("e", 0, false));
    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0, true));

    parser->endGroup();

    BOOST_CHECK_EQUAL(3, parser->symbol("a", 0, false));
    BOOST_CHECK_EQUAL(3, parser->symbol("a", 0, true));

    BOOST_CHECK_EQUAL(0, parser->symbol("d", 0, false));
    BOOST_CHECK_EQUAL(0, parser->symbol("d", 0, true));

    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0, false));
    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0, true));
}

BOOST_AUTO_TEST_CASE( parser_parse )
{
    shared_ptr<Parser> parser = create_parser("abc{def}gh");
    parser->lexer()->setCatcode('{', Token::CC_BGROUP);
    parser->lexer()->setCatcode('}', Token::CC_EGROUP);
    
    Node::ptr document = parser->parse();
    std::cout << document->treeRepr();
}

