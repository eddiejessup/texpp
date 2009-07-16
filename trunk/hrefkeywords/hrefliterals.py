#!/usr/bin/env python

import sys
import texpy

import latexstubs

import codecs
import re
import os

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

def isLocalFile(filename, workdir = ''):
    w = os.path.abspath(workdir)
    return w == os.path.commonprefix((w, os.path.abspath(filename)))

def parseDocument(filename, fileobj):
    """ Parses the document using TeXpp """
    parser = texpy.Parser(filename, fileobj, '', False, True)

    # Mimic the most important parts of LaTeX style
    latexstubs.initLaTeXstyle(parser)

    # Do the real work
    return parser.parse()

def scanDocument(node, literals, words, stemmer,
                    replace = '\\href{%(concept)s}{%(text)s}',
                    workdir = '',
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
            replaced - a dict {filename->source} after replacement
                       (or empty string if replace argument is None)
    """

    if maxChars is None:
        maxChars = max(len(c) for c in literals.keys())

    childrenCount = node.childrenCount()
    stats = {}
    replaced = {}

    # Do nothing for leaf nodes
    if childrenCount == 0:
        for f, s in node.sources():
            replaced.setdefault(f, []).append(s)
        return (stats, replaced)


    n = 0
    while n < childrenCount:
        child = node.child(n)

        # Try replacing text_word or text_character
        if child.type() in ('text_word', 'text_character') and \
                child.isOneFile() and isLocalFile(child.oneFile(), workdir):
            cur_text = ''
            child_file = child.oneFile()
            start_line, start_pos, end_line, end_pos = child.sourcePos()
            found_literals = []
            for m in xrange(n, childrenCount):
                childm = node.child(m)

                # look in literals dictionary
                if childm.type() in ('text_word', 'text_character', 'text_space') \
                                            and childm.isOneFile():
                    if childm.oneFile() != child_file:
                        break

                    cur_text += childm.value()
                    childm_pos = childm.sourcePos()
                    if start_line == 0:
                        start_line = childm_pos[0]
                        start_pos = childm_pos[1]
                    if childm_pos[2] != 0:
                        end_line = childm_pos[2]
                        end_pos = childm_pos[3]

                    # ignore spaces at the end
                    if childm.type() != 'text_space':
                        cur_literal = normLiteral(cur_text, words, stemmer)
                        if len(cur_literal) > maxChars:
                            break

                        cur_concept = literals.get(cur_literal, None)
                        if cur_concept is not None:
                            found_literals.append((cur_concept, cur_text, m,
                                                        end_line, end_pos))
            
                # break on anything else
                else:
                    break

            if found_literals:
                # do replace
                c = found_literals[-1]
                concept = c[0][0]

                if replace is not None:
                    replaced.setdefault(child_file, []) \
                        .append(replace % {'concept': concept, 'text': c[1]})
                        
                # save replace positions
                stats.setdefault(concept, [])
                stats[concept].append((os.path.abspath(child_file),
                                start_line, start_pos, c[3], c[4]))
                n = c[2]

            elif replace is not None:
                # nothing found
                for f in child.sources():
                    replaced.setdefault(f.key(), []).append(f.data())

        # Walk recursively if whitelisted
        elif child.type() in latexstubs.whitelistEnvironments:
            (replaced1, stats1) = scanDocument(child,
                              literals, words, stemmer, replace, maxChars)
            for f, s in replaced1:
                replaced.setdefault(f, []).append(s)
            for c,s in stats1.iteritems():
                stats.setdefault(c, [])
                stats[c].extend(s)

        # Just grab the source otherwise
        elif replace is not None:
            for f in child.sources():
                replaced.setdefault(f.key(), []).append(f.data())

        n += 1

    replaced1 = {}
    for f, s in replaced.iteritems():
        replaced1[f] = ''.join(s)

    return (stats, replaced1)

def main():
    """ Main routine """

    # Define command line options
    from optparse import OptionParser
    optparser = OptionParser(usage='%prog [options] texfile')
    optparser.add_option('-c', '--concepts', type='string', help='concepts file')
    optparser.add_option('-o', '--output', type='string', help='output directory')
    optparser.add_option('-m', '--macro', type='string', default='href', 
                                    help='macro (default: href)')
    optparser.add_option('-s', '--stats', action='store_true', help='print stats')

    # Parse command line options
    opt, args = optparser.parse_args()

    # Additional options verification
    if opt.concepts is None:
        optparser.error('Required option --concepts was not specified')

    if opt.output is None:
        optparser.error('Required option --output was not specified')

    if len(args) != 1:
        optparser.error('Wrong command line arguments')

    # Open input file
    try:
        filename = args[0]
        fileobj = open(filename, 'r')
    except IOError, e:
        optparser.error('Can not open input file (\'%s\'): %s' % \
                                (filename, str(e)))

    # Open concepts file
    try:
        conceptsfile = open(opt.concepts, 'r')
    except IOError, e:
        optparser.error('Can not open concepts file (\'%s\'): %s' % \
                                (opt.concepts, str(e)))

    # Check output dir
    if not os.path.exists(opt.output):
        try:
            os.mkdir(opt.output)
        except (IOError, OSError), e:
            optparser.error('Can not create output directory (\'%s\'): %s' % \
                                (opt.output, str(e)))

    if not os.path.isdir(opt.output):
        optparser.error('\'%s\' is not a directory')

    # Load words and create stemmer
    words = loadWords()
    stemmer = createStemmer()

    # Load concepts
    literals = loadLiteralsFromConcepts4(conceptsfile, words, stemmer)
    conceptsfile.close()

    # Load and parse the document
    document = parseDocument(filename, fileobj)

    # Do the main job
    (stats, replaced) = scanDocument(document, literals, words, stemmer)

    for f, s in replaced.iteritems():
        if isLocalFile(f):
            fname = os.path.join(opt.output, f)
            try:
                outfile = open(fname, 'w')
            except IOError, e:
                sys.stdout('Can not open output file (\'%s\'): %s' % \
                                    (fname, str(e)))
                continue
            outfile.write(s)
            outfile.close()

    fileobj.close()

    # Stats
    if opt.stats:
        for w,n in stats.iteritems():
            print 'Concept <%s> replaced %d times' % (w, len(n))

    print stats

if __name__ == '__main__':
    main()

