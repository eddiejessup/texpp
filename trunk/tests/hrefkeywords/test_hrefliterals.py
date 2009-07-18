#!/usr/bin/env python

import sys
sys.path.append(sys.argv[1])
sys.path.append(sys.argv[2])
sys.path.append(sys.argv[3])
sys.argv = sys.argv[0:1] + sys.argv[4:]

import hrefliterals
import StringIO
import unittest
import os

class NormLiteralTest(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(NormLiteralTest, self).__init__(*args, **kwargs)
        self.stemmer = hrefliterals.Stemmer()
        self.words = hrefliterals.WordsDict(
                                    '/usr/share/dict/words', 4)
        self.words.insert('I')
        self.words.insert('a')

    def normLiteral(self, literal):
        return hrefliterals.normLiteral(
                literal, self.words, self.stemmer)

    def testLongWords(self):
        self.assertEqual(self.normLiteral('electrons'), 'electron')
        self.assertEqual(self.normLiteral('xxxxxxxxx'), 'xxxxxxxxx')
        self.assertEqual(self.normLiteral('eLectrons'), 'E.L.E.C.T.R.O.N.S.')

    def testShortWords(self):
        self.assertEqual(self.normLiteral('or'), 'or')
        self.assertEqual(self.normLiteral('set'), 'set')
        self.assertEqual(self.normLiteral('sets'), 'set')
        self.assertEqual(self.normLiteral('SET'), 'S.E.T.')
        self.assertEqual(self.normLiteral('S.E.T.'), 'S.E.T.')
        self.assertEqual(self.normLiteral('s.e.t.'), 'S.E.T.')
        self.assertEqual(self.normLiteral('seT'), 'S.E.T.')

    def testShortNonWords(self):
        self.assertEqual(self.normLiteral('dof'), 'D.O.F.')
        self.assertEqual(self.normLiteral('Dof'), 'D.O.F.')
        self.assertEqual(self.normLiteral('DoF'), 'D.O.F.')

    def testShortNames(self):
        self.assertEqual(self.normLiteral('Ada'), 'ada')
        self.assertEqual(self.normLiteral('ada'), 'A.D.A.')

    def testOneLetter(self):
        self.assertEqual(self.normLiteral('I'), 'i')
        self.assertEqual(self.normLiteral('i'), 'I.')
        self.assertEqual(self.normLiteral('A'), 'a')
        self.assertEqual(self.normLiteral('a'), 'a')
        self.assertEqual(self.normLiteral('T'), 'T.')
        self.assertEqual(self.normLiteral('t'), 'T.')

    def testSAbbr(self):
        self.assertEqual(self.normLiteral('Dr.A.B.'), 'DR.A.B.')
        self.assertEqual(self.normLiteral('Dr.A.Boy.'), 'DR.A.BOY.')
        self.assertEqual(self.normLiteral('Dr.A.Boy'), 'DR.A.boi')
        self.assertEqual(self.normLiteral('Dr.A.boy'), 'DR.A.boi')
        self.assertEqual(self.normLiteral('Dr.A.Ada'), 'DR.A.ada')
        self.assertEqual(self.normLiteral('Dr.A.ada'), 'DR.A.A.D.A.')
        self.assertEqual(self.normLiteral('Dr.A.B.'), 'DR.A.B.')

    def testDigits(self):
        self.assertEqual(self.normLiteral('Fig.1'), 'FIG.1.')
        self.assertEqual(self.normLiteral('SU(2)'), 'S.U.(2.)')

    def testIgnoredSymbols(self):
        self.assertEqual(self.normLiteral('g-factor'), 'G.factor')
        self.assertEqual(self.normLiteral('g/factor'), 'G.factor')
        self.assertEqual(self.normLiteral('g.factor'), 'G.factor')
        self.assertEqual(self.normLiteral('g factor'), 'G.factor')
        self.assertEqual(self.normLiteral('T-J model'), 'T.J.model')

    def testPluralAbbr(self):
        self.assertEqual(self.normLiteral('SETs'), 'S.E.T.')
        self.assertEqual(self.normLiteral('SETes'), 'S.E.T.')
        self.assertEqual(self.normLiteral('SETS'), 'S.E.T.S.')
        self.assertEqual(self.normLiteral('SETES'), 'S.E.T.E.S.')
        self.assertEqual(self.normLiteral('S.E.T.s'), 'S.E.T.S.')
        self.assertEqual(self.normLiteral('dofs'), 'D.O.F.S.')

    def testApostrophe(self):
        self.assertEqual(self.normLiteral('world\'s'), 'world')

    def testMultiWords(self):
        self.assertEqual(self.normLiteral('hello world'), 'helloworld')
        self.assertEqual(self.normLiteral('hello-world'), 'helloworld')
        self.assertEqual(self.normLiteral('hello/world'), 'helloworld')
        self.assertEqual(self.normLiteral('hello+world'), 'hello+world')
        self.assertEqual(self.normLiteral('test SET'), 'testS.E.T.')

class ScanDocumentText(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(ScanDocumentText, self).__init__(*args, **kwargs)
        self.stemmer = hrefliterals.Stemmer()
        self.words = hrefliterals.WordsDict(
                                    '/usr/share/dict/words', 4)
        self.words.insert('I')
        self.words.insert('a')

    def testScanDocument(self):
        literals = {'concept': ['concept']}
        texfile = StringIO.StringIO('Some text with some concepts inside')
        document = hrefliterals.parseDocument('texfile', texfile)
        stats, replaced = hrefliterals.scanDocument(document,
                                literals, self.words, self.stemmer,
                                replace = '{%(concept)s}{%(text)s}')

        self.assertEqual(stats,
            {'concept': [(os.path.abspath('texfile'), 1, 20, 1, 28)]})
        self.assertEqual(replaced,
            {'texfile': 'Some text with some {concept}{concepts} inside'})

if __name__ == '__main__':
    unittest.main()

