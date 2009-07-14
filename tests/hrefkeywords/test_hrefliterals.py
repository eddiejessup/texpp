#!/usr/bin/env python

import sys
sys.path.append(sys.argv[1])
sys.path.append(sys.argv[2])
sys.argv = sys.argv[0:1] + sys.argv[3:]

import hrefliterals
import StringIO
import unittest

class NormLiteralTest(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(NormLiteralTest, self).__init__(*args, **kwargs)
        self.words = hrefliterals.loadWords()
        self.stemmer = hrefliterals.createStemmer()

    def normLiteral(self, literal):
        return hrefliterals.normLiteral(literal, self.words, self.stemmer)

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
        self.assertEqual(self.normLiteral('Dr.A.Boy'), 'DR.A.boy')
        self.assertEqual(self.normLiteral('Dr.A.boy'), 'DR.A.boy')
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

class AddLiteralTest(unittest.TestCase):
    def testAddLiteral(self):
        l = {}
        hrefliterals.addLiteral(l, u'aa', 1)
        hrefliterals.addLiteral(l, u'bb', 2)
        self.assertEqual(set(hrefliterals.listLiterals(l)),
                            set(['aa', 'bb']))

        self.assertEqual(hrefliterals.findLiteral(l, u'aa'), 1)
        self.assertEqual(hrefliterals.findLiteral(l, u'bb'), 2)
        self.assertEqual(hrefliterals.findLiteral(l, u'cc'), None)

        hrefliterals.addLiteral(l, u'c', 3)
        hrefliterals.addLiteral(l, u'a', 4)
        self.assertEqual(set(hrefliterals.listLiterals(l)),
                            set(['aa', 'bb', 'c', 'a']))

        hrefliterals.addLiteral(l, u'aaff', 1)
        hrefliterals.addLiteral(l, u'cffff', 2)
        self.assertEqual(set(hrefliterals.listLiterals(l)),
                            set(['aa', 'bb', 'c', 'a', 'aaff', 'cffff']))

        self.assertEqual(hrefliterals.findLiteral(l, u'aa'), 1)
        self.assertEqual(hrefliterals.findLiteral(l, u'bb'), 2)
        self.assertEqual(hrefliterals.findLiteral(l, u'a'), 4)
        self.assertEqual(hrefliterals.findLiteral(l, u'aaff'), 1)
        self.assertEqual(hrefliterals.findLiteral(l, u'cc'), None)

class ScanDocumentText(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(ScanDocumentText, self).__init__(*args, **kwargs)
        self.words = hrefliterals.loadWords()
        self.stemmer = hrefliterals.createStemmer()

    def testScanDocument(self):
        literals = {'concept': ['concept']}
        texfile = StringIO.StringIO('Some text with some concepts inside')
        document = hrefliterals.parseDocument('texfile', texfile)
        stats, replaced = hrefliterals.scanDocument(document, 'texfile',
                                literals, self.words, self.stemmer,
                                replace = '{%(concept)s}{%(text)s}')

        self.assertEqual(stats.keys(), ['concept'])
        self.assertEqual(len(stats['concept']), 1)
        self.assertEqual(len(stats['concept'][0]), 2)
        self.assertEqual(stats['concept'][0][0].value(), 'concepts')
        self.assertEqual(stats['concept'][0][1].value(), 'concepts')
        self.assertEqual(replaced,
                'Some text with some {concept}{concepts} inside')

if __name__ == '__main__':
    unittest.main()
