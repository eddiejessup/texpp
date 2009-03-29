#!/usr/bin/python

import os
import sys
import shutil
import urllib2
import random
import optparse
import gzip
import tarfile
import StringIO
import re

THISDIR = os.path.dirname(__file__)

TESTHREF_DEF = r"""
\def\testhref#1#2{\immediate\write16{REPLACE}\relax#2}
"""

class CommandError(Exception):
    pass

class TestError(Exception):
    SUCCESS = 0
    UNPACK_FAILED = 1
    NO_TEX_FILE = 2
    AUTO_HYPERREF_FAILED = 3
    TEST_PROGRAM_FAILED = 4
    ORIG_LATEX_FAILED = 5
    REPL_LATEX_FAILED = 6
    ORIG_NO_DVI = 7
    REPL_NO_DVI = 8
    DVI_DIFFERS = 9

    ET_SUCCESS = 0
    ET_ERROR = 1
    ET_FAIL = 2

    def __init__(self, message, code, etype):
        super(TestError, self).__init__(message)
        self.code = code
        self.etype = etype

    def result(self):
        if self.etype == self.ET_SUCCESS:
            return 'success'
        elif self.etype == self.ET_ERROR:
            return 'error'
        elif self.etype == self.ET_FAIL:
            return 'fail'

def unpack_article_from_arxiv(filename, outdir, fileobj=None):
    if os.path.exists(outdir):
        shutil.rmtree(outdir, True)

    os.mkdir(outdir)

    if fileobj is None:
        try:
            fileobj = file(filename, 'r')
        except IOError:
            return False

    try:
        gzipobj = gzip.GzipFile(fileobj=fileobj)
    except IOError:
        return False 
        
    try:
        tarobj = tarfile.TarFile(fileobj=gzipobj)
        # Mutliple files
        for name in tarobj.getnames():
            nname = os.path.normpath(name)
            if os.path.isabs(nname) or nname.startswith('..'):
                return False
        tarobj.extractall(outdir)

    except tarfile.TarError:
        # Single file
        gzipobj.seek(0)
        outfile = file(os.path.join(outdir, 'main.tex'), 'w')
        outfile.write(gzipobj.read())
        outfile.close()
        gzipobj.close()

    return True

def find_main_latex_file(dirname):
    for fname in os.listdir(dirname):
        # Only consider files with .tex extensions
        if fname.endswith('.tex'):
            fnamefull = os.path.join(dirname, fname)
            # Look for \documentclass in file
            fobj = file(fnamefull, 'r')
            for line in fobj:
                if line.find('\\documentclass')>=0 or \
                        line.find('\\documentstyle')>=0:
                    fobj.close()
                    return fname
            fobj.close()
    return None

def fix_latex_hyperref(filename, testhref=False):
    # Create backup file
    shutil.copy(filename, filename + '.bak')
    fobj = file(filename + '.bak', 'r')
    dobj = file(filename, 'w')

    if testhref:
        data = fobj.read()
        dobj.write(TESTHREF_DEF)
        dobj.write(data)
        return True

    # Check wether hyperref is already included
    hyperref_found = False
    hyperref_re = re.compile(
        r'^([^%]*\\usepackage\s*)(?:\[([^]]*)\])?(\s*{\s*hyperref\s*})')
    end_re = re.compile(r'^[^%]*\\end\s*{\s*document\s*}')
    for line in fobj:
        m = hyperref_re.match(line)
        if m:
            if m.group(2):
                if m.group(2).find('breaklinks') < 0:
                    line = hyperref_re.sub(r'\1[breaklinks,\2]\3', line)
            else:
                line = hyperref_re.sub(r'\1[breaklinks]\3', line)
            hyperref_found = True

        if not hyperref_found and end_re.match(line):
            break

        dobj.write(line)

    if not hyperref_found:
        # We have to insert hyperref ourselfs
        fobj.seek(0)
        dobj.seek(0)
        dobj.truncate()
        documentclass_re = re.compile(
            r'^[^%]*\\documentclass\s*([^]]*])?\s*{\s*[^}]*\s*}')
        for line in fobj:
            dobj.write(line)
            if documentclass_re.match(line):
                dobj.write('\\usepackage[breaklinks]{hyperref}\n')
                hyperref_found = True

    fobj.close()
    dobj.close()

    if not hyperref_found:
        raise CommandError('Can not automatically insert hyperref package')

    return True

def compile_latex(filename, verbose=False):
    oldcwd = os.getcwd()
    os.chdir(os.path.dirname(filename))

    basename = os.path.basename(filename)
    jobname = os.path.splitext(basename)[0]

    # XXX: do we need to rerun latex multiple times ?
    if verbose:
        latex_cmd = 'latex -interaction=nonstopmode "%s"' % (basename,)
    else:
        latex_cmd = 'latex -interaction=batchmode "%s" > /dev/null' % (
                                                             basename,)
    ret = os.system(latex_cmd)
        
    os.chdir(oldcwd)
    if ret:
        raise CommandError('latex error: %d' % (ret,))

def test_one_file(fname, opt):
    # Prepare the work dir
    wdir = os.path.join(opt.work_dir, fname)
    if os.path.exists(wdir):
        shutil.rmtree(wdir, True)

    os.mkdir(wdir)

    # Unpack the file
    unpackdir = os.path.join(wdir, 'unpack')
    if not unpack_article_from_arxiv(
                os.path.join(opt.tex_dir, fname), unpackdir):
        raise TestError('Can not unpack the article',
                        TestError.UNPACK_FAILED, TestError.ET_ERROR)

    # Find main tex file
    texfile = find_main_latex_file(unpackdir)
    if not texfile:
        raise TestError('Can not find main TeX file in the archive',
                        TestError.NO_TEX_FILE, TestError.ET_ERROR)

    # Prepare dirs for test
    origdir = os.path.join(wdir, 'orig')
    repldir = os.path.join(wdir, 'repl')
    origfile = os.path.join(origdir, texfile)
    replfile = os.path.join(repldir, texfile)

    shutil.copytree(unpackdir, origdir)
    shutil.copytree(unpackdir, repldir)

    # Add hyperref package or fix its options
    try:
        fix_latex_hyperref(origfile, not opt.real_href)
    except CommandError, e:
        raise TestError('Can not automatically insert hyperref package',
                        TestError.AUTO_HYPERREF_FAILED, TestError.ET_ERROR)

    # Prepare replacer command
    if opt.real_href:
        macro = 'href'
    else:
        macro = 'testhref'

    replacer_cmd = opt.cmd % { 'macro': macro,
                               'source': '"' + origfile + '"',
                               'output': '"' + replfile + '"' }
    os.system('echo "***" %s >> %s' % (replacer_cmd, opt.cmd_log_file))

    if not opt.verbose:
        replacer_cmd += ' 2>&1 >> ' + opt.cmd_log_file
    else:
        replacer_cmd += ' 2>&1 | tee -a ' + opt.cmd_log_file

    # Run replacer
    ret = os.system(replacer_cmd)
    if ret:
        raise TestError('Error running test program',
                        TestError.TEST_PROGRAM_FAILED, TestError.ET_FAIL)

    # Compile original
    try:
        compile_latex(origfile, opt.verbose)
    except CommandError, e:
        raise TestError('Can not compile original document %s: %s' % (
            origfile, e), TestError.ORIG_LATEX_FAILED, TestError.ET_ERROR)

    # Compile replaced
    try:
        compile_latex(replfile, opt.verbose)
    except CommandError, e:
        raise TestError('Can not compile modified document %s: %s' % (
            replfile, e), TestError.REPL_LATEX_FAILED, TestError.ET_FAIL)

    # Compare DVIs only in testhref mode
    if not opt.real_href:
        # Read DVIs
        try:
            origdvi = file(os.path.splitext(origfile)[0] + '.dvi', 'r')
        except IOError,e:
            raise TestError('Can not open original dvi file for %s: %s' % (
                origfile, e), TestError.ORIG_NO_DVI, TestError.ET_ERROR)
        try:
            repldvi = file(os.path.splitext(replfile)[0] + '.dvi', 'r')
        except IOError,e:
            raise TestError('Can not open modified dvi file for %s: %s' % (
                origfile, e), TestError.REPL_NO_DVI, TestError.ET_FAIL)

        # Skip the first 100 bytes (it contains a timestamp)
        origdvi.read(100)
        repldvi.read(100)

        # Compile DVIs
        while 1:
            odata = origdvi.read(1024)
            rdata = repldvi.read(1024)
            if odata != rdata:
                raise TestError('Generated dvi files differ',
                    TestError.DVI_DIFFERS, TestError.ET_FAIL)
            if not odata:
                break

        origdvi.close()
        repldvi.close()

    return 'success'

def main(argv):
    optparser = optparse.OptionParser()

    cmd_default = './hrefkeywords.py -c ./concepts4.txt ' + \
                '-s -m %(macro)s -o %(output)s %(source)s'

    optparser.add_option('-d', '--download', metavar='NUM', type='int',
                help='Download NUM articles from arxiv.org to TEX_DIR')
    optparser.add_option('-t', '--tex-dir', default='tex',
                help='Directory with original articles from arxiv.org')
    optparser.add_option('-w', '--work-dir', default='work',
                help='Directory for working files')
    optparser.add_option('-l', '--log-file', default='test.log',
                help='File to log test results')
    optparser.add_option('-L', '--cmd-log-file', default='test_cmd.log',
                help='Log file for command output')
    optparser.add_option('-m', '--macros-dir', default='texmacros',
                help='Folder with extra TeX macros from arxiv.org')
    optparser.add_option('-r', '--real-href', action='store_true',
                help='Use real href instead of a fake macro')
    optparser.add_option('-v', '--verbose', action='store_true',
                help='Show program output')
    optparser.add_option('-c', '--cmd', default=cmd_default,
                help='Replacer command (default=%s)' % (cmd_default,))

    opt, args = optparser.parse_args(argv)

    os.environ['TEXINPUTS'] = os.environ.get('TEXINPUTS', '.:') + ':' + \
                                os.path.abspath(opt.macros_dir)

    if opt.download is not None:
        for x in xrange(opt.download):
            download_random_from_arxiv(opt.tex_dir, sys.stdout)

    if opt.cmd_log_file:
        try:
            os.remove(opt.cmd_log_file)
        except OSError:
            pass

    testcount = 0
    testerror = 0
    testfail = 0
    logfile = file(opt.log_file, 'w')

    files = os.listdir(opt.tex_dir)
    filescount = len(files)
    filenum = 0

    for fname in files:
        filenum += 1

        sys.stdout.write('[%d of %d] %s:' % (
                        filenum, filescount, fname,))
        sys.stdout.flush()
        if opt.verbose:
            sys.stdout.write('\n')

        testcount += 1
        logfile.write('%s: ' % (fname,))
        logfile.flush()

        try:
            result = test_one_file(fname, opt)
            etype = TestError.ET_SUCCESS
        except TestError, e:
            result = e.result() + ': ' + e.message
            etype = e.etype

        logfile.write(result + '\n')
        logfile.flush()

        if opt.verbose:
            sys.stdout.write(' ...')

        sys.stdout.write(' %s\n' % (result,))
    
        if etype == TestError.ET_ERROR:
            testerror += 1
        elif etype == TestError.ET_FAIL:
            testfail += 1

    msg = '%d (of %d) tests failed, %d tests broken' % (
            testfail, testcount - testerror, testerror)
    if logfile:
        logfile.write(msg + '\n')

    sys.stdout.write(msg + '\n')

if __name__ == '__main__':
    main(sys.argv)

