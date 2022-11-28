import re


class Command(object):
    
    @classmethod
    def read_from(cls, line):
        tokens = re.sub(' +', ' ', line).split(' ')
        tokens = list(map(lambda token: token.lower(), tokens))

        if not tokens:
            raise EmptyCommand()
        
        cmd, args = tokens[0], tokens[1:]
        
        subclasses = cls.__subclasses__()
        for subclass in subclasses:
            if subclass.handles(cmd):
                try:
                    return subclass(args)
                except Exception:
                    raise InvalidArguments(cmd, args)
                
        raise CommandNotFound(cmd)
    
    @classmethod
    def handles(cls, cmd):
        return cmd == cls.name()
    
    @classmethod
    def name(cls):
        raise NotImplementedError
    
    def execute_on(self, client):
        raise NotImplementedError
    
    
class Init(Command):
    
    @classmethod
    def name(cls):
        return 'init'    
    
    def __init__(self, args):
        self._user = args[0]
        self._value = float(args[1])
        self._bits = int(args[2])
    
    def execute_on(self, client):
        return client.init(self._user, self._value, self._bits)


class Load(Command):
    
    @classmethod
    def name(cls):
        return 'load'    
    
    def __init__(self, args):
        self._filename = args[0]
    
    def execute_on(self, client):
        return client.load(self._filename)


class Save(Command):
    
    @classmethod
    def name(cls):
        return 'save'    
    
    def __init__(self, args):
        self._filename = args[0]
    
    def execute_on(self, client):
        return client.save(self._filename)
    
    
class Balance(Command):
    
    @classmethod
    def name(cls):
        return 'balance'    
    
    def __init__(self, args):
        self._user = args[0]
    
    def execute_on(self, client):
        return client.balance(self._user)    


class Transfer(Command):
    
    @classmethod
    def name(cls):
        return 'transfer'    
    
    def __init__(self, args):
        self._src = args[0]
        self._dst = dict()
        for i in range(1, len(args), 2):
            dst = args[i]
            value = float(args[i+1])
            self._dst[dst] = value
    
    def execute_on(self, client):
        return client.transfer(self._src, self._dst)
    
    
class Mine(Command):
    
    @classmethod
    def name(cls):
        return 'mine'    
    
    def __init__(self, args):
        self._bits = int(args[0])
    
    def execute_on(self, client):
        return client.mine(self._bits)    


class Block(Command):
    
    @classmethod
    def name(cls):
        return 'block'    
    
    def __init__(self, args):
        self._id = args[0]
    
    def execute_on(self, client):
        return client.block(self._id)    


class Transaction(Command):
    
    @classmethod
    def name(cls):
        return 'transaction'    
    
    def __init__(self, args):
        self._id = args[0]
    
    def execute_on(self, client):
        return client.transaction(self._id)    
    
    
class InvalidCommand(Exception):
    
    pass


class EmptyCommand(InvalidCommand):
    
    def __init__(self):
        self.message = 'command is empty'


class InvalidArguments(InvalidCommand):
    
    def __init__(self, cmd, args):
        self.message = f'invalid arguments for {cmd}'


class CommandNotFound(InvalidCommand):
    
    def __init__(self, cmd):
        self.message = f'command {cmd} not found'