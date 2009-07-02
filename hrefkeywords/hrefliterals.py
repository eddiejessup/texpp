#!/usr/bin/env python

#import sys
#import texpy

#import latexstubs

import codecs
import re

re_cut = re.compile(r'[-/ ]|(?:\'s(?=[^A-Za-z0-9.]))')
re_word = re.compile(r'[a-zA-Z0-9]+(?:[a-zA-Z0-9.]*\.)?')
re_symbol = re.compile(r'[^a-zA-Z0-9]')
re_abbr = re.compile(r'[0-9.]|.[A-Z]')

ABBR_MAX = 4

def normLiteral(literal, words, stemmer):
    """ Return normalized representation of literal.

        Arguments:
            words   A dictionary of short English words
            stemmer An English stemmer
    """

    n = 0
    sliteral = ''

    while n < len(literal):
        m = re_cut.match(literal[n:])
        if m:
            # Next symbol(s) should be ignored
            n += m.end()
            continue

        m = re_symbol.match(literal[n:])
        if m:
            # Next symbol should be saved as-is
            sliteral += m.group(0)
            n += m.end()
            continue

        else:
            # Next symbol is part of a word
            m = re_word.match(literal[n:]); assert m
            w = m.group(0)

            if re_abbr.search(w) or \
                    (len(w) <= ABBR_MAX and \
                        (w not in words and w.lower() not in words)):
                # Case 1: abbrevation
                
                if '.' not in w:
                    # Stem plural forms for uppercase abbrevations without dots
                    if w.endswith('es') and w[:-2].isupper():
                        w = w[:-2]
                    elif w.endswith('s') and w[:-1].isupper():
                            w = w[:-1]

                    # Insert dots for abbrevations without dots
                    w = reduce(lambda a,b: a+b+'.', w, '')

                # Convert to uppercase
                sliteral += w.upper()

            else:
                # Case 2: normal word
                # Convert to lowercase and stem
                sliteral += stemmer.stem(w.lower())

        n += m.end()

    return sliteral

def loadWords():
    words = set(['I', 'a'])
    words_file = codecs.open('/usr/share/dict/words', 'r', 'latin1')
    for w in words_file:
        w = w.strip()
        if len(w) > 1 and len(w) <= ABBR_MAX and not w.endswith('\'s'):
            words.add(w)
    words_file.close()
    return words

def createStemmer():
    from nltk.stem.porter import PorterStemmer
    return PorterStemmer()

if __name__ == '__main__':
    import doctest
    doctest.testmod()

