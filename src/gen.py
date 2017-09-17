from jinja2 import Template
from enum import Enum

class Type(Enum):
    UINT = 0
    INT = 1
    CHARP = 2
    FILEP = 3
    VOIDP = 4
    SIZET = 5

    def c_type(self):
        if self.value == Type.UINT.value: return 'unsigned int'
        elif self.value == Type.INT.value: return 'int'
        elif self.value == Type.CHARP.value: return 'char*'
        elif self.value == Type.FILEP.value: return 'FILE*'
        elif self.value == Type.VOIDP.value: return 'void*'
        elif self.value == Type.SIZET.value: return 'size_t'

    def lua_to(self):
        if self.value == Type.UINT.value: return 'lua_tointeger'
        elif self.value == Type.INT.value: return 'lua_tointeger'
        elif self.value == Type.CHARP.value: return 'lua_tostring'
        elif self.value == Type.FILEP.value: return 'lua_topointer'
        elif self.value == Type.VOIDP.value: return 'lua_topointer'
        elif self.value == Type.SIZET.value: return 'lua_tointeger'

    def lua_is(self):
        if self.value == Type.UINT.value: return 'lua_isnumber'
        elif self.value == Type.INT.value: return 'lua_is_number'
        elif self.value == Type.CHARP.value: return 'lua_isstring'
        elif self.value == Type.FILEP.value: return 'lua_islightuserdata'
        elif self.value == Type.VOIDP.value: return 'lua_islightuserdata'
        elif self.value == Type.SIZET.value: return 'lua_isnumber'

    def lua_push(self):
        if self.value == Type.UINT.value: return 'lua_pushinteger'
        elif self.value == Type.INT.value: return 'lua_pushinteger'
        elif self.value == Type.CHARP.value: return 'lua_pushstring'
        elif self.value == Type.FILEP.value: return 'lua_pushlightuserdata'
        elif self.value == Type.VOIDP.value: return 'lua_pushlightuserdata'
        elif self.value == Type.SIZET.value: return 'lua_pushinteger'


class Arg(object):
    def __init__(self, type, name):
        self.type = type
        self.name = name

    def declaration(self):
        return '%s %s'%(self.type.c_type(), self.name)

class Function(object):
    def __init__(self, name, ret, args):
        self.name = name
        self.ret = ret
        self.args = args

    def args_declaration(self):
        return ', '.join(map(lambda x: x.declaration(), self.args))

    def args_types(self):
        return ', '.join(map(lambda x: x.type.c_type(), self.args))

    def args_names(self):
        return ', '.join(map(lambda x: x.name, self.args))

def generate(functions):
    with open('main.c.template', 'r') as f:
        t = Template(f.read())
        ret = t.render(
            functions=functions,
        )
        return ret

def main():
    functions = [
        Function('sleep', Type.UINT, [
            Arg(Type.UINT, 'seconds'),
        ]),
        Function('fgets', Type.CHARP, [
            Arg(Type.CHARP, 'str'),
            Arg(Type.INT, 'num'),
            Arg(Type.FILEP, 'stream'),
        ]),
        Function('malloc', Type.VOIDP, [
            Arg(Type.SIZET, 'size'),
        ]),
    ]
    print(generate(functions))

if __name__ == '__main__':
    main()
