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
    nliteral = ''

    while n < len(literal):
        m = re_cut.match(literal[n:])
        if m:
            # Next symbol(s) should be ignored
            n += m.end()
            continue

        m = re_symbol.match(literal[n:])
        if m:
            # Next symbol should be saved as-is
            nliteral += m.group(0)
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
                nliteral += w.upper()

            else:
                # Case 2: normal word
                # Convert to lowercase and stem
                nliteral += stemmer.stem(w.lower())

        n += m.end()

    return nliteral

def addLiteral(literals, literal, id):
    assert(type(literals) is dict)
    assert(type(literal) is unicode)
    assert(id and (type(id) is int))

    if not literals or (len(literals) == 1 and literals.keys() == [True]):
        literals.update({literal: {True: id}})
        return

    try:
        l = len(literals.keys()[0])
    except TypeError:
        l = len(literals.keys()[1])
        
    if l == len(literal):
        literals.setdefault(literal, {})[True] = id

    elif l > len(literal):
        literals_old = dict(literals)
        literals.clear()
        l = len(literal)
        for k, v in literals_old.iteritems():
            literals.setdefault(k[:l], {})[k[l:]] = v
        addLiteral(literals, literal, id)

    else: # i.e. l < len(literal)
        addLiteral(literals.setdefault(literal[:l], {}), literal[l:], id)

def listLiterals(literals, prefix=u'', ret=[]):
    for k, v in literals.iteritems():
        if k is True:
            ret.append(prefix)
        else:
            ret.extend(listLiterals(v, prefix+k))
    return ret

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

