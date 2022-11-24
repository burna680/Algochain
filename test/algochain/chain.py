import copy

from collections import OrderedDict, defaultdict

from .hash import Hasher, MerkleTreeHash
from .txn import\
    RawOutput, RawInput, RawTransaction,\
    Transaction, Input, Output, NullOutput
from .block import RawBlock, Block, NullBlock


class Algochain(object):
    
    NULL_ID = '0'*64
    
    
    @classmethod
    def from_file(cls, filename):
        reader = Reader()
        return reader.read(filename)
    
    def __init__(self):
        self._blocks = OrderedDict()
        self._blocks_by_height = list()
        self._txns = OrderedDict()
        self._utxos = defaultdict(lambda: list())
        self._validator = Validator.for_chain(self)
    
    def add_block(self, raw_block):
        block = self._validator.validate(raw_block)
        
        self._blocks[block.id()] = block
        self._blocks_by_height.append(block)
        
        for txn in block:
            self._txns[txn.id()] = txn
            self._update_utxos(txn)
            
    def height(self):
        return len(self._blocks_by_height)
    
    def get_block(self, id=None, height=None):
        if id is None and height is None:
            raise RuntimeError('must provide block id or block height')
        
        if id is not None:
            if id not in self._blocks:
                raise RuntimeError(f'invalid block ID: {id}')            
            return self._blocks[id]
        elif height is not None:
            if height < 0 or height >= len(self._blocks_by_height):
                raise RuntimeError(f'invalid block height: {height}')             
            return self._blocks_by_height[height]
    
    def get_txn(self, txid):
        if txid not in self._txns:
            raise RuntimeError(f'invalid transaction ID: {txid}')
        
        return self._txns[txid]
    
    def has_txn(self, txid):
        return txid in self._txns
    
    def get_utxos(self, addr=None):
        if addr is None:
            return copy.deepcopy(self._utxos)
        else:
            return self._utxos[addr]
    
    def block_count(self):
        return len(self._blocks)
    
    def tip(self):
        if not self._blocks_by_height:
            return None
        
        return self._blocks_by_height[-1]
    
    def serialize(self):
        return ''.join([block.serialize() for block in self._blocks.values()])
    
    def __iter__(self):
        return iter(self._blocks.values())
    
    def _update_utxos(self, txn):
        for input in txn.inputs():
            prev_output = input.prev_output()
            if not prev_output.is_null():
                addr_utxos = self._utxos[prev_output.addr()]
                addr_utxos.remove(prev_output)
                
        for output in txn.outputs():
            self._utxos[output.addr()].append(output)
            
            
class Validator(object):
    
    @classmethod
    def for_chain(cls, chain):
        return cls(chain)
    
    @classmethod
    def hash_method(cls):
        return MerkleTreeHash
    
    @classmethod
    def is_header_valid(cls, raw_block):
        hdr_hash = Hasher.hash(raw_block.header().serialize())
        bits = cls.count_leading_zeros(hdr_hash)
        return bits >= raw_block.bits()   
    
    @classmethod
    def is_txns_hash_valid(cls, raw_block, method=None):
        txns_hash = cls.get_txns_hash(raw_block.txns(), method)
        return raw_block.txns_hash() == txns_hash
    
    @classmethod
    def count_leading_zeros(cls, string):
        count = 0
        i = 0
        zeros = {
            '1': 3, '2': 2, '3': 2, '4': 1,
            '5': 1, '6': 1, '7': 1, '8': 0,
            '9': 0, 'a': 0, 'b': 0, 'c': 0,
            'd': 0, 'e': 0, 'f': 0                         
        }
        
        while string[i] == '0':
            count += 4
            i += 1
        
        return count + zeros[string[i]]    
    
    @classmethod
    def get_txns_hash(cls, txns, method=None):
        method = method or cls.hash_method()
        return method(txns).value()      

    def __init__(self, chain):
        self._chain = chain
    
    def validate(self, raw_block):
        if not self.is_header_valid(raw_block):
            raise InvalidBlockHeader()
        
        if not self.is_txns_hash_valid(raw_block):
            raise InvalidTransactionHash()
        
        if self._chain.block_count() == 0:
            prev_block, txns = self._validate_genesis(raw_block)
        else:
            prev_block, txns = self._validate_block(raw_block)
            
        return Block(prev_block, raw_block.header(), txns)
    
    def _validate_genesis(self, raw_block):
        if raw_block.prev_block() != Algochain.NULL_ID:
            raise InvalidGenesisFormat()
        
        if raw_block.txn_count() != 1:
            raise InvalidGenesisFormat()
        
        raw_txn = raw_block.txns()[0]
        if raw_txn.input_count() != 1 or raw_txn.output_count() != 1:
            raise InvalidGenesisFormat()
        
        txn_input = raw_txn.inputs()[0]
        if txn_input.txid() != Algochain.NULL_ID or\
            txn_input.idx() != 0 or\
            txn_input.addr() != Algochain.NULL_ID:
            raise InvalidGenesisFormat()
        
        txn_output = raw_txn.outputs()[0]
        
        input = Input(NullOutput())
        output = Output(txn_output.addr(), txn_output.value())
        txn = Transaction([input], [output])
        
        return NullBlock(), [txn]
    
    def _validate_block(self, raw_block):
        input_sum = output_sum = 0
        current_txns = dict()
        current_txns_list = list()
        utxos = self._chain.get_utxos()
        
        for raw_txn in raw_block.txns():
            inputs = list()
            outputs = list()
            
            for input_raw in raw_txn.inputs():
                txid = input_raw.txid()
                if not self._chain.has_txn(txid) and txid not in current_txns:
                    raise InvalidOutpointID(txid)
                
                if self._chain.has_txn(txid):
                    prev_txn = self._chain.get_txn(txid)
                else:
                    prev_txn = current_txns[txid]
                    
                idx = input_raw.idx()
                if idx < 0 or idx >= prev_txn.output_count():
                    raise InvalidOutpointIndex(idx)
            
                addr = input_raw.addr()
                prev_output = prev_txn.get_output(idx)
                if addr != prev_output.addr():
                    raise InvalidInputAddress(addr)
                
                # Check that the output is an UTXO
                addr_utxos = utxos[addr]
                is_utxo = False
                for i, utxo in enumerate(addr_utxos):
                    if utxo.txn().id() == txid and utxo.idx_in_txn() == idx:
                        is_utxo = True
                        del addr_utxos[i]
                        break
                    
                if not is_utxo:
                    raise OutputAlreadySpent(txid, idx)
                
                inputs.append(Input(prev_output))
                input_sum += prev_output.numeric_value()
                
            for raw_output in raw_txn.outputs():
                if raw_output.numeric_value() < 0:
                    raise InvalidTransactionAmount()
                
                output = Output(raw_output.addr(), raw_output.value())
                
                utxos[output.addr()].append(output)
                outputs.append(output)
                
                output_sum += output.numeric_value()
                
            if input_sum < output_sum:
                raise NotEnoughFunds()
            
            txn = Transaction(inputs, outputs)
            current_txns[txn.id()] = txn
            current_txns_list.append(txn)
            
        prev_block = self._chain.get_block(id=raw_block.prev_block())
        
        return prev_block, current_txns_list
    
    
class Miner(object):
    
    def __init__(self, method=None):
        self._method = method or Validator.hash_method()
        
    def mine(self, txns, prev_block, difficulty):
        txn_count = len(txns)
        txns_hash = Validator.get_txns_hash(txns, method=self._method)
        candidate = RawBlock(
                        prev_block,
                        txns_hash,
                        difficulty,
                        0,
                        txn_count,
                        txns)
        
        while not Validator.is_header_valid(candidate):
            candidate._header._nonce += 1
            
        return candidate        
    
    
class Reader(object):
    
    def read(self, filename):
        with open(filename, 'r') as _file:
            lines = _file.readlines()
            
        return self._read_from(lines)
    
    def _read_from(self, lines):
        self._algochain = Algochain()
        offset = 0
        
        while offset < len(lines):
            if not lines[offset].strip():
                offset += 1
                continue
            raw_block, offset = self._read_block(lines, offset)
            self._algochain.add_block(raw_block)
            
        return self._algochain
    
    def _read_block(self, lines, offset):
        if offset+4 >= len(lines):
            raise RuntimeError(f'line {offset}: invalid block data')
        
        prev_block = lines[offset].strip()
        txns_hash = lines[offset+1].strip()
        bits = self._to_int(lines[offset+2], offset+2)
        nonce = self._to_int(lines[offset+3], offset+3)
        
        txn_count = self._to_int(lines[offset+4], offset+4)
        txns = list()
        txn_offset = offset+5
        for _ in range(txn_count):
            txn = self._read_txn(lines, txn_offset)
            txns.append(txn)
            txn_offset += 2 + txn.input_count() + txn.output_count()
        
        return RawBlock(
                    prev_block, txns_hash, bits, nonce,
                    txn_count, txns), txn_offset
    

    def _check_txn_offset(self, lines, offset):
        if offset >= len(lines):
            raise RuntimeError(f'line {offset}: invalid transaction data')

    def _read_txn(self, lines, offset):
        self._check_txn_offset(lines, offset)
        input_count = self._to_int(lines[offset], offset)
        ins = list()
        offset += 1
        for _ in range(input_count):
            self._check_txn_offset(lines, offset)
            txid, idx, addr = lines[offset].split(' ')
            _in = RawInput(txid.strip(), self._to_int(idx, offset), addr.strip())
            ins.append(_in)
            offset += 1
        
        self._check_txn_offset(lines, offset)
        output_count = self._to_int(lines[offset], offset)
        outs = list()
        offset += 1
        for _ in range(output_count):
            self._check_txn_offset(lines, offset)
            value, addr = lines[offset].split(' ')
            self._to_float(value, offset)
            _out = RawOutput(value, addr.strip())
            outs.append(_out)
            offset += 1
        
        return RawTransaction(input_count, ins, output_count, outs)
    
    def _to_int(self, value, line_no):
        try:
            return int(value)
        except Exception:
            raise ValueError(f'line {line_no}: found {value}, expected integer')

    def _to_float(self, value, line_no):
        try:
            return float(value)
        except Exception:
            raise ValueError(f'line {line_no}: found {value}, expected float')
        
        
class ValidationException(Exception):
    
    def __str__(self):
        return self.message


class InvalidGenesisFormat(ValidationException):
    
    def __init__(self):
        self.message = 'invalid format for genesis block'
        
        
class InvalidBlockHeader(ValidationException):
    
    def __init__(self):
        self.message = 'invalid block header'


class InvalidTransactionHash(ValidationException):
    
    def __init__(self):
        self.message = 'invalid transaction hash'
        
        
class InvalidOutpointID(ValidationException):
    
    def __init__(self, txid):
        self.message = f'invalid outpoint ID: {txid}'


class InvalidOutpointIndex(ValidationException):
    
    def __init__(self, idx):
        self.message = f'invalid outpoint index: {idx}'


class InvalidInputAddress(ValidationException):
    
    def __init__(self, addr):
        self.message = f'invalid input address: {addr}'


class OutputAlreadySpent(ValidationException):
    
    def __init__(self, txid, idx):
        self.message = f'output already spent: ({txid}, {idx})'


class NotEnoughFunds(ValidationException):
    
    def __init__(self):
        self.message = 'insufficient funds in transaction'
        
        
class InvalidTransactionAmount(ValidationException):
    
    def __init__(self):
        self.message = 'invalid transaction amount'
