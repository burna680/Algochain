from .object import AlgochainObject


class BlockHeader(object):
    
    def __init__(self, prev_block, txns_hash, bits, nonce):
        self._prev_block = prev_block
        self._txns_hash = txns_hash
        self._bits = bits
        self._nonce = nonce
        
    def prev_block(self):
        return self._prev_block
    
    def txns_hash(self):
        return self._txns_hash
        
    def bits(self):
        return self._bits
    
    def nonce(self):
        return self._nonce
    
    def serialize(self):
        return f'{self._prev_block}\n{self._txns_hash}\n{self._bits}\n{self._nonce}\n'


class AbstractBlock(AlgochainObject):
        
    def header(self):
        return self._header
    
    def prev_block(self):
        return self._header.prev_block()
    
    def txns_hash(self):
        return self._header.txns_hash()
        
    def bits(self):
        return self._header.bits()
    
    def nonce(self):
        return self._header.nonce()
    
    def txn_count(self):
        return len(self._txns)
    
    def txns(self):
        return self._txns
    
    def get_txn(self, idx):
        if idx >= self.txn_count():
            raise IndexError(f'invalid transaction index for block {self.id()}')
        
        return self._txns[idx]

    def serialize(self):
        header = self._header.serialize()
        
        txns = str().join(map(lambda txn: txn.serialize(), self._txns))
        
        body = f'{self.txn_count()}\n{txns}'
        
        return f'{header}{body}'
    
    def __iter__(self):
        return iter(self._txns)    
    
    
class RawBlock(AbstractBlock):
    
    def __init__(self,
                 prev_block, txns_hash, bits, nonce,
                 txn_count, txns):
        self._header = BlockHeader(prev_block, txns_hash, bits, nonce)
        self._txn_count = txn_count
        self._txns = txns
    
    
class Block(AbstractBlock):
    
    def __init__(self, prev, header, txns):
        self._prev_block = prev
        self._header = header
        self._txns = txns
        
        for txn in txns:
            txn._set_block(self)
            
    def is_null(self):
        return False            
    
    def previous(self):
        return self._prev_block
    
        
class NullBlock(Block):
    
    def __init__(self):
        self._txns = list()
        self._prev_block = self
        
    def is_null(self):
        return True
    
    def id(self):
        from .chain import Algochain
        return Algochain.NULL_ID
