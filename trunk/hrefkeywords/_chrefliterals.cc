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

#include <boost/python.hpp>
#include <set>
#include <string>
#include <fstream>

#include <cstring>
#include <cctype>

#include <iostream>

extern "C" {
struct stemmer;

extern struct stemmer * create_stemmer(void);
extern void free_stemmer(struct stemmer * z);

extern int stem(struct stemmer * z, char * b, int k);
}

namespace {

using std::string;

class Stemmer {
public:
    Stemmer() { _stemmer = create_stemmer(); }
    ~Stemmer() { free_stemmer(_stemmer); }

    string stem(string word) const
    {
        /* TODO: remove unnessesary conversion to/from std::string */
        int n = word.size();
        char buf[n];
        std::memcpy(buf, word.data(), n);
        n = ::stem(_stemmer, buf, n-1);
        return string(buf, n+1);
    }

protected:
    mutable struct stemmer* _stemmer;
};

class WordsDict {
public:
    WordsDict(string filename, size_t abbrMaxLen)
        : _abbrMaxLen(abbrMaxLen)
    {
        std::ifstream wordsfile(filename.c_str());
        string word;
        while(wordsfile.good()) {
            std::getline(wordsfile, word);
            size_t s = word.size();
            if(s > 1 && s <= abbrMaxLen &&
                    (s < 2 || word.substr(s-2) != "'s") ) {
                _words.insert(word);
            }
        }
    }

    size_t abbrMaxLen() const { return _abbrMaxLen; }

    void insert(string word) { _words.insert(word); }
    bool contains(string word) const { return _words.count(word); }

protected:
    std::set<string> _words;
    size_t _abbrMaxLen;
};

inline bool _islower(char ch) { return ch >= 'a' && ch <= 'z'; }
inline bool _isupper(char ch) { return ch >= 'A' && ch <= 'Z'; }

string normLiteral(string literal,
        const WordsDict* wordsDict, const Stemmer* stemmer)
{
    /* TODO: support arbitrary stemmers and wordDicts */
    /* TODO: unicode and locales */
    string nliteral;
    size_t n = 0, s = literal.size();
    size_t wordStart = string::npos;
    size_t lastDot = string::npos;

    /* TODO: handle 's */
    for(n=0; ; ++n) {
        char ch = n < s ? literal[n] : 0;

        if(wordStart < n) { // inside a word
            if(_islower(ch) || _isupper(ch) || std::isdigit(ch)) {
                // add to current word
            } else if(ch == '.') {
                // add to current word but remember the dot position
                lastDot = n;
            } else { // end of the word

                // re-read current char next time
                --n;
                
                // check for the dot
                if(lastDot < n) {
                    // dot is present but not the last char
                    n = lastDot;
                }

                // extract and lower the word
                bool isAbbr = false;
                size_t lastUpper = string::npos;
                size_t firstLower = string::npos;
                size_t wordSize = n + 1 - wordStart;
                std::string word(literal, wordStart, wordSize);
                std::string word1(literal, wordStart, wordSize);

                for(size_t k = 0; k < wordSize; ++k) {
                    if(_isupper(word1[k])) {
                        word1[k] += ('a' - 'A');
                        lastUpper = k;
                        if(k != 0) isAbbr = true;
                    } else if(_islower(word1[k])) {
                        if(firstLower > k) firstLower = k;
                    } else { // digit or dot
                        isAbbr = true;
                    }
                }

                // check for abbr in dictionary
                if(!isAbbr && wordSize <= wordsDict->abbrMaxLen()) {
                    isAbbr = !wordsDict->contains(word);
                    if(isAbbr && lastUpper == 0)
                        isAbbr = !wordsDict->contains(word1);
                }
                
                // process the word
                if(isAbbr) {
                    if(lastDot > n) {
                        // Stem plural forms for uppercase abbrevations
                        if(wordSize > 2 && firstLower == wordSize-2 &&
                                word[wordSize-2] == 'e' &&
                                word[wordSize-1] == 's') {
                            --wordSize; --wordSize;
                            word.resize(wordSize);
                        } else if(wordSize > 1 && firstLower == wordSize-1 &&
                                word[wordSize-1] == 's') {
                            word.resize(--wordSize);
                        }
                        size_t nliteralSize = nliteral.size();
                        nliteral.resize(nliteralSize + 2*wordSize);
                        for(size_t k=0; k<wordSize; ++k) {
                            nliteral[nliteralSize + (k<<1)] =
                                _islower(word[k]) ?
                                    word[k] - ('a' - 'A') : word[k];
                            nliteral[nliteralSize + (k<<1) + 1] = '.';
                        }
                    } else {
                        size_t nliteralSize = nliteral.size();
                        nliteral.resize(nliteralSize + wordSize);
                        for(size_t k=0; k<wordSize; ++k) {
                            nliteral[nliteralSize + k] =
                                _islower(word[k]) ?
                                    word[k] - ('a' - 'A') : word[k];
                        }
                    }
                } else {
                    nliteral += stemmer->stem(word1);
                }
                
                // reset the word
                wordStart = lastDot = string::npos;
            }
        } else { // not inside a word
            if(ch == ' ' || ch == '-' || ch == '/') {
                continue; // ignore these chars
            } else if(_islower(ch) || _isupper(ch) || std::isdigit(ch)) {
                wordStart = n;
            } else if(ch == '\'' && n+1 < s && (literal[n+1] == 's')) {
                if(n+2==s) break;
                char ch2 = literal[n+2];
                if(_islower(ch2) || _isupper(ch2) || std::isdigit(ch2)
                                                    || ch2 == '.')
                    nliteral += literal[n];
                else
                    ++n;
            } else if(n >= s) {
                break;
            } else {
                nliteral += literal[n]; // use character as-is
            }
        }
    }

    return nliteral;
}

} // namespace

BOOST_PYTHON_MODULE(_chrefliterals)
{
    using namespace boost::python;
    class_<Stemmer>("Stemmer", init<>())
        .def("stem", &Stemmer::stem)
    ;

    class_<WordsDict>("WordsDict", init<string, size_t>())
        .def("insert", &WordsDict::insert)
        .def("contains", &WordsDict::contains)
    ;

    def("normLiteral", &normLiteral);
}

