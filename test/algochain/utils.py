import random

from .chain import Validator, Reader, Miner
from .hash import Hasher, SequentialHash
from .txn import RawTransaction, RawInput, RawOutput
from .proto.random import RandomGenerator


MAX_TXNS = 2000
MAX_INS  = 200
MAX_OUTS = 200

DEFAULT_PREV_BLOCK = 'f'*64


def random_commands(cmd_file, chain_file=None, n=None):
    generator = RandomGenerator()
    chain = generator.generate(n)
    cmds = generator.commands()
    balances = generator.balances()
    
    for user,balance in balances.items():
        print(f'{user}: {balance}')
    
    with open(cmd_file, 'w') as _file:
        _file.write('\n'.join(cmds))
        
    if chain_file is not None:
        with open(chain_file, 'w') as _file:
            _file.write(chain.serialize())        

def random_txns(filename, n=None):
    n = n or random.randint(0, MAX_TXNS)
    txns = [_random_txn() for _ in range(n)]
    
    content = str().join(map(lambda txn: txn.serialize(), txns))
    
    with open(filename, 'w') as _file:
        _file.write(content)
        
def is_valid_block(block_arg, method=None):
    raw_block = block_arg
    method = method or SequentialHash
    
    try:
        if isinstance(block_arg, str):
            raw_block = _read_block(block_arg)
            
        txns_hash_valid = Validator.is_txns_hash_valid(raw_block, method=method)
        header_valid = Validator.is_header_valid(raw_block)
        
        return txns_hash_valid and header_valid
    except Exception as exc:
        raise RuntimeError(f'Failed to process block: {str(exc)}') from None
    
def build_block(txns_arg, bits, prev_block=None, filename=None, method=None):
    txns = txns_arg
    prev_block = prev_block or DEFAULT_PREV_BLOCK
    method = method or SequentialHash
    miner = Miner(method)
    
    try:
        if isinstance(txns_arg, str):
            txns = _read_txns(txns_arg)
            
        block = miner.mine(txns, prev_block, bits)
    except Exception:
        raise RuntimeError('Failed to process transactions!')
    else:
        if filename is not None:
            with open(filename, 'w') as _file:
                _file.write(block.serialize())
        
        return block
    
def _random_txn():
    input_count = random.randint(0, MAX_INS)
    ins = [_random_input() for _ in range(input_count)]

    output_count = random.randint(0, MAX_OUTS)
    outs = [_random_output() for _ in range(output_count)]
    
    return RawTransaction(input_count, ins, output_count, outs)
    
def _random_input():
    txid = _random_hash()
    idx = random.randint(0, MAX_OUTS)
    addr = _random_hash()
    
    return RawInput(txid, idx, addr)

def _random_output():
    value = random.randint(0, MAX_TXNS) + random.random()
    addr = _random_hash()
    
    return RawOutput(value, addr)

def _random_hash():
    return Hasher.hash(random.randint(0, MAX_TXNS**3))

def _read_block(filename):
    with open(filename, 'r') as _file:
        lines = _file.readlines()    
    
    block, offset = Reader()._read_block(lines, 0)
    
    if offset < len(lines):
        raise Exception('block has garbage after last transaction.')
    
    return block

def _read_txns(filename):
    with open(filename, 'r') as _file:
        lines = _file.readlines() 

    txns = list()
    offset = 0
    while offset < len(lines):
        txn = Reader()._read_txn(lines, offset)
        txns.append(txn)
        offset += 2 + txn.input_count() + txn.output_count()
        
    return txns