from .client import Client
from .command import Command, InvalidCommand

__all__ = ['terminal', 'from_file']


def terminal():
    client = Client()
    
    while True:
        try:
            line = input('> ')
        except EOFError:
            break
        
        _parse_cmd_and_run(client, line)
                

def from_file(filename):
    with open(filename, 'r') as _file:
        lines = _file.readlines()
        
    client = Client()
        
    for line in lines:
        _parse_cmd_and_run(client, line.strip())
        
        
def _parse_cmd_and_run(client, line):
    try:
        cmd = Command.read_from(line)
    except InvalidCommand as exc:
        print(f'FAIL: {exc.message}')
    else:
        output = cmd.execute_on(client)
        
        if output.ok():
            print(output.resource())
        else:
            print(f'FAIL: {output.reason()}')    