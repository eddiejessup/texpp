#!/usr/bin/env python

import sys
import texpy

import latexstubs

def main():
    """ Main routine """
    # Define command line options
    from optparse import OptionParser
    optparser = OptionParser(usage='%prog [options] texfile')
    optparser.add_option('-t', '--print-tree', action='store_true',
                                    help='print document tree before exit')

    # Parse command line options
    opt, args = optparser.parse_args()

    if len(args) == 0:
        # Input from terminal
        interactive = True
        filename = ''
        fileobj = None
    elif len(args) == 1:
        # Open input file
        interactive = False
        filename = args[0]
        try:
            filename = args[0]
            fileobj = open(filename, 'r')
        except IOError:
            optparser.error('Can not open input file (\'%s\')' % (filename,))
    else:
        optparser.error('Wrong command line arguments')
        
    # Create the parser
    parser = texpy.Parser(filename, fileobj, interactive,
                            texpy.ConsoleLogger())
    doc = parser.parse()
    
    # Print tree
    if opt.print_tree:
        print doc.treeRepr()

if __name__ == '__main__':
    main()

