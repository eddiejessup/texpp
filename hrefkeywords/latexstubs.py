import texpy

class BeginCommand(texpy.Command):
    def invoke(self, parser, node):
        group = parser.parseGroup(texpy.Parser.GroupType.NORMAL)
        node.appendChild('env_type', group)

        env_type = ''
        for n,c in group.children():
            if n == 'text_word':
                env_type = c.value()
                break

        parser.beginCustomGroup('environment_' + env_type)
        return True

class EndCommand(texpy.Command):
    def invoke(self, parser, node):
        group = parser.parseGroup(texpy.Parser.GroupType.NORMAL)
        node.appendChild('env_type', group)

        env_type = ''
        for n,c in group.children():
            if n == 'text_word':
                env_type = c.value()
                break

        if env_type == 'end':
            parser.end()

        parser.endCustomGroup()
        return True

class Newcommand(texpy.Command):
    def invoke(self, parser, node):
        node.appendChild('cmd', parser.parseGeneralText())
        node.appendChild('optional_spaces', parser.parseOptionalSpaces())

        if parser.peekToken() and parser.peekToken().isCharacter('['):
            args = texpy.Node('newcommand_args')
            node.appendChild('args', args)
            while parser.peekToken():
                parser.nextToken(args.tokens())
                if parser.lastToken().isCharacter(']'):
                    break
        
        if parser.peekToken() and parser.peekToken().isCharacter('['):
            opt = texpy.Node('newcommand_opt')
            node.appendChild('opt', opt)
            while parser.peekToken():
                parser.nextToken(opt.tokens())
                if parser.lastToken().isCharacter(']'):
                    break

        node.appendChild('def', parser.parseGeneralText())

        return True

class DefCommand(texpy.Command):
    def invoke(self, parser, node):
        node.appendChild('token', parser.parseControlSequence())
        args = texpy.Node('def_args')
        node.appendChild('args', args)
        while parser.peekToken() and \
                not parser.peekToken().isCharacterCat(
                            texpy.Token.CatCode.BGROUP):
            parser.nextToken(args.tokens())
        
        node.appendChild('def', parser.parseGeneralText(False))

        return True

    def checkPrefixes(self, parser):
        return True

def createCommand(parser, name, cmd, *args):
    parser.setSymbol('\\' + name, cmd('\\' + name, *args))

def initLaTeXstyle(parser):
    """ Defines the most important aspects of LaTeX style """

    parser.setSymbol('catcode'+str(ord('{')),
                int(texpy.Token.CatCode.BGROUP))
    parser.setSymbol('catcode'+str(ord('}')),
                int(texpy.Token.CatCode.EGROUP))
    parser.setSymbol('catcode'+str(ord('$')),
                int(texpy.Token.CatCode.MATHSHIFT))
    parser.setSymbol('catcode'+str(ord('\t')),
                int(texpy.Token.CatCode.SPACE))

    createCommand(parser, 'begin', BeginCommand)
    createCommand(parser, 'end', EndCommand)
    createCommand(parser, 'newcommand', Newcommand)
    createCommand(parser, 'renewcommand', Newcommand)
    createCommand(parser, 'providecommand', Newcommand)

    createCommand(parser, 'def', DefCommand)
    createCommand(parser, 'edef', DefCommand)
    createCommand(parser, 'gdef', DefCommand)
    createCommand(parser, 'xdef', DefCommand)

    mathToken = texpy.Token(texpy.Token.Type.CHARACTER,
                            texpy.Token.CatCode.MATHSHIFT, '$')
    parser.setSymbol('\\(', texpy.TokenCommand(mathToken))
    parser.setSymbol('\\)', texpy.TokenCommand(mathToken))
    parser.setSymbol('\\[', texpy.TokenCommand(mathToken))
    parser.setSymbol('\\]', texpy.TokenCommand(mathToken))

whitelistEnvironments = frozenset((
    'environment_document',
    'environment_abstract',
    'environment_itemize',
    'environment_enumerate',
    'environment_description',
    'environment_list',
    'environment_theorem',
    'environment_center'
))


