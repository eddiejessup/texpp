import texpy

class BeginCommand(texpy.Command):
    def invoke(self, parser, node):
        print "BeginCommand.invoke()"
        return False
    def texRepr(self, c):
        print "BeginCommand.texRepr()"
        return 'xxx'


