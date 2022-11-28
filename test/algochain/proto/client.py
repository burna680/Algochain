import collections

from algochain.chain import Algochain, Miner, NotEnoughFunds, InvalidTransactionAmount
from algochain.hash import Hasher
from algochain.txn import RawInput, RawOutput, RawTransaction


UTXO = collections.namedtuple('UTXO', ['txid', 'idx', 'value'])

def protocol_method(func):
    def function(self, *args, **kwargs):
        try:
            value = func(self, *args, **kwargs)
            ret = Success('OK' if value is None else value)
        except Exception as exc:
            ret = Failure(str(exc))
        return ret
    return function


class Client(object):
    
    def __init__(self):
        self._current_txns = list()
        self._utxos = collections.defaultdict(lambda: list())
        self._miner = Miner()
        self._chain = None
        
    def _get_addr(self, user):
        return Hasher.hash(user)
    
    def _build_genesis(self, value, addr, bits):
        input = RawInput(Algochain.NULL_ID, 0, Algochain.NULL_ID)
        output = RawOutput(value, addr)
        txn = RawTransaction(1, [input], 1, [output])
        return self._miner.mine(
                txns=[txn],
                prev_block=Algochain.NULL_ID,
                difficulty=bits)
    
    def _build_txn(self, src_addr, dst_dict, utxos):
        inputs = list()
        total = 0
        
        for utxo in utxos:
            txid, idx, value = utxo
            input = RawInput(txid, idx, src_addr)
            inputs.append(input)
            total += value
        
        outputs = list()
        used = 0
        for dst_user,amount in dst_dict.items():
            if amount == 0:
                continue
            if amount < 0:
                raise InvalidTransactionAmount()
            
            dst_addr = self._get_addr(dst_user)
            output = RawOutput(amount, dst_addr)
            outputs.append(output)
            used += amount
            
        change = total - used
        if change > 0:
            output = RawOutput(change, src_addr)
            outputs.append(output)
            
        return RawTransaction(len(inputs), inputs, len(outputs), outputs)
    
    def _reset_utxos(self):
        utxos = self._chain.get_utxos()
        self._utxos = collections.defaultdict(lambda: list())
        for addr,addr_utxos in utxos.items():
            addr_utxos = [UTXO(
                            txid=utxo.txn().id(),
                            idx=utxo.idx_in_txn(),
                            value=utxo.numeric_value())
                          for utxo in addr_utxos]
            
            self._utxos[addr] = addr_utxos
            
    def _check_chain_initialized(self):
        if self._chain is None:
            raise RuntimeError('chain not initialized')
    
    def chain(self):
        return self._chain

    @protocol_method
    def init(self, user, value, bits=None):
        self._chain = Algochain()
        
        bits = bits or 10
        addr = self._get_addr(user)
        raw_block = self._build_genesis(value, addr, bits)
        
        self._chain.add_block(raw_block)
        self._reset_utxos()
        
        return self._chain.tip().id()
        
    @protocol_method
    def transfer(self, src, dst):
        self._check_chain_initialized()
        
        src_addr = self._get_addr(src)
        target_amount = sum(dst.values())
        
        if target_amount <= 0:
            raise InvalidTransactionAmount()
        
        chosen_utxos = list()
        current_amount = 0
        
        for utxo in self._utxos[src_addr]:
            if current_amount >= target_amount:
                break
            
            chosen_utxos.append(utxo)
            current_amount += utxo.value
            
        if current_amount < target_amount:
            raise NotEnoughFunds()
        
        txn = self._build_txn(src_addr, dst, chosen_utxos)
        self._current_txns.append(txn)
        
        for utxo in chosen_utxos:
            self._utxos[src_addr].remove(utxo)

        txid = txn.id()
        for idx, raw_output in enumerate(txn.outputs()):
            utxo = UTXO(txid=txid, idx=idx, value=raw_output.numeric_value())
            self._utxos[raw_output.addr()].append(utxo)
            
        return txid
        
    @protocol_method
    def mine(self, bits):
        self._check_chain_initialized()
        
        tip = self._chain.tip()
        
        raw_block = self._miner.mine(
                        txns=self._current_txns,
                        prev_block=tip.id(),
                        difficulty=bits)
        
        self._chain.add_block(raw_block)
        
        self._current_txns = list()
        self._reset_utxos() 
        
        return self._chain.tip().id()
    
    @protocol_method
    def balance(self, user):
        self._check_chain_initialized()
        
        addr = self._get_addr(user)
        return sum([utxo.value for utxo in self._utxos[addr]])
    
    @protocol_method
    def block(self, block_id):
        self._check_chain_initialized()
        
        return self._chain.get_block(id=block_id)
    
    @protocol_method
    def transaction(self, id):
        self._check_chain_initialized()
        
        return self._chain.get_txn(id)

    @protocol_method    
    def save(self, filename):
        self._check_chain_initialized()
        
        with open(filename, 'w') as _file:
            _file.write(self._chain.serialize())
         
    @protocol_method   
    def load(self, filename):
        self._chain = Algochain.from_file(filename)
        
        self._reset_utxos()
        
        return self._chain.tip().id()
        
        
class ReturnValue(object):
    
    def ok(self):
        raise NotImplementedError


class Success(ReturnValue):
    
    def __init__(self, resource):
        self._resource = resource
        
    def resource(self):
        return self._resource
    
    def ok(self):
        return True
    
    
class Failure(ReturnValue):
    
    def __init__(self, reason):
        self._reason = reason
        
    def reason(self):
        return self._reason
    
    def ok(self):
        return False