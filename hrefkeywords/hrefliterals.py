#!/usr/bin/env python

import sys
import texpy

import latexstubs

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

def loadLiteralsFromConcepts4(conceptsfile, words, stemmer):
    """ Loads concepts from the files
        and arranges them in a dictionary """
    literals = {}
    for line in conceptsfile:
        line = line.strip(' \n\r')
        line1 = line.lower() \
                    .replace('\\-', '-') \
                    .replace('\\(', '(') \
                    .replace('\\)', ')')

        literals.setdefault(normLiteral(line1, words, stemmer), []) \
                .append(line1)

    return literals

def parseDocument(filename, fileobj):
    """ Parses the document using TeXpp """
    parser = texpy.Parser(filename, fileobj, '', False, True)

    # Mimic the most important parts of LaTeX style
    latexstubs.initLaTeXstyle(parser)

    # Do the real work
    return parser.parse()

def scanDocument(node, filename, literals, words, stemmer,
                    replace = '\\href{%(concept)s}{%(text)s}',
                    maxChars = None):
    """ Find literals in a parsed document

        Arguments:
            node     - document node
            filename - document file name
            literals - literals dictionary
            replace  - pattern to replace found literals
            maxChars - maximum lendth of the normed literal

        Returns a tuple (stats, replaced)
            stats    - a dictionary which maps each found concept
                       to a tuple (first_node, last_node)
            replaced - document source after replacement
                       (or empty string if replace argument is None)
    """

    if maxChars is None:
        maxChars = max(len(c) for c in literals.keys())

    childrenCount = node.childrenCount()

    # Do nothing for leaf nodes
    if childrenCount == 0:
        return (node.source(filename), {})

    src = []
    n = 0
    stats = {}
    while n < childrenCount:
        child = node.child(n)

        # Try replacing text_word or text_character
        if child.type() in ('text_word', 'text_character'):
            cur_text = ''
            found_literals = []
            for m in xrange(n, childrenCount):
                childm = node.child(m)

                # look in literals dictionary
                if childm.type() in ('text_word', 'text_character'):
                    cur_text += childm.value()
                    cur_literal = normLiteral(cur_text, words, stemmer)
                    if len(cur_literal) > maxChars:
                        break

                    cur_concept = literals.get(cur_literal, None)
                    if cur_concept is not None:
                        found_literals.append((cur_concept, cur_text, m))
            
                # skip spaces
                elif childm.type() == 'text_space':
                    cur_text += childm.value()

                # break on anything else
                else:
                    break

            if found_literals:
                # do replace
                c = found_literals[-1]
                concept = c[0][0]

                if replace is not None:
                    src.append(replace % {'concept': concept, 'text': c[1]})
                        
                stats.setdefault(concept, [])
                stats[concept].append((child, node.child(c[2])))
                n = c[2]

            elif replace is not None:
                # nothing found
                src.append(child.source(filename))

        # Walk recursively if whitelisted
        elif child.type() in latexstubs.whitelistEnvironments:
            (src1, stats1) = scanDocument(child, filename,
                              literals, words, stemmer, replace, maxChars)
            src.append(src1)
            for c,s in stats1.iteritems():
                stats.setdefault(c, [])
                stats[c].extend(s)

        # Just grab the source otherwise
        elif replace is not None:
            src.append(child.source(filename))

        n += 1

    return (stats, ''.join(src))

