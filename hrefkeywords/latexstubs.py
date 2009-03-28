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

def initLaTeXstyle(parser):
    """ Defines the most important aspects of LaTeX style """

    parser.setSymbol('\\end', None)
    parser.setSymbol('catcode'+str(ord('{')), 1)
    parser.setSymbol('catcode'+str(ord('}')), 2)
    parser.setSymbol('catcode'+str(ord('$')), 3)

    parser.setSymbol('\\begin', BeginCommand('\\begin'))
    parser.setSymbol('\\end', EndCommand('\\end'))

    mathToken = texpy.Token(texpy.Token.Type.CHARACTER,
                            texpy.Token.CatCode.MATHSHIFT, '$')
    parser.setSymbol('\\(', texpy.TokenCommand(mathToken))
    parser.setSymbol('\\)', texpy.TokenCommand(mathToken))

whitelistEnvironments = frozenset((
    'environment_document',
    'environment_itemize',
    'environment_enumerate',
    'environment_description',
    'environment_list',
    'environment_theorem',
    'environment_center'
))


