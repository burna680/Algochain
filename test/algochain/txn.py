from .object import AlgochainObject


class AbstractInput(object):
    
    def txid(self):
        return self._txid
    
    def idx(self):
        return self._idx
    
    def addr(self):
        return self._addr
    
    
class RawInput(AbstractInput):
    
    def __init__(self, txid, idx, addr):
        self._txid = txid
        self._idx = idx
        self._addr = addr
        

class Input(AbstractInput):
    
    def __init__(self, output):
        self._output = output
        self._txid = output.txn().id()
        self._addr = output.addr()
        self._idx = output.idx_in_txn()
        self._txn = None
        
    def _set_txn(self, txn):
        self._txn = txn
        
    def txn(self):
        return self._txn
    
    def prev_output(self):
        return self._output
        

class AbstractOutput(object):
    
    def value(self):
        return self._value
    
    def numeric_value(self):
        return float(self._value)
    
    def addr(self):
        return self._addr   
    
    
class RawOutput(AbstractOutput):
    
    def __init__(self, value, addr):
        self._value = value
        self._addr = addr
    
    
class Output(AbstractOutput):
    
    def __init__(self, addr, value):
        self._addr = addr
        self._value = value
        self._txn = None
        self._idx_in_txn = None
    
    def _set_txn(self, txn, idx):
        self._txn = txn
        self._idx_in_txn = idx
        
    def txn(self):
        return self._txn
    
    def idx_in_txn(self):
        return self._idx_in_txn
    
    def is_null(self):
        return False    
        
    def __eq__(self, other):
        if not isinstance(other, Output):
            return False
        
        if self._txn is None and other._txn is not None or\
           self._txn is not None and other._txn is None:
            return False
            
        return\
            self._txn is None and other._txn is None or\
            (self.txn().id() == other.txn().id() and\
            self.value() == other.value() and\
            self.addr() == other.addr())
            
    def __hash__(self):
        if self._txn is None:
            return\
                hash(None) ^\
                hash(self.value()) ^\
                hash(self.addr())
        else:
            return\
                hash(self.txn().id()) ^\
                hash(self.value()) ^\
                hash(self.addr())
            
            
class NullOutput(Output):

    def __init__(self):
        from .chain import Algochain
        Output.__init__(self, Algochain.NULL_ID, 0)
        self._txn = NullTransaction()
        self._idx_in_txn = 0
        
    def is_null(self):
        return True
        

class AbstractTransaction(AlgochainObject):
    
    def input_count(self):
        return len(self.inputs())
    
    def output_count(self):
        return len(self.outputs())
    
    def inputs(self):
        return self._inputs
    
    def outputs(self):
        return self._outputs
    
    def get_input(self, idx):
        if idx < 0 or idx >= self.input_count():
            raise IndexError(f'invalid input index for transaction {self.id()}')
            
        return self._inputs[idx]
    
    def get_output(self, idx):
        if idx < 0 or idx >= self.output_count():
            raise IndexError(f'invalid output index for transaction {self.id()}')
            
        return self._outputs[idx]    
        
    def serialize(self):
        ins = str().join(
                [f'{_in.txid()} {_in.idx()} {_in.addr()}\n'
                 for _in in self._inputs]
            )
        
        outs = str().join(
                [f'{_out.value()} {_out.addr()}\n'
                for _out in self._outputs]
            )
        
        return '{n_in}\n{ins}{n_out}\n{outs}'.format(
            n_in=self.input_count(),
            ins=ins,
            n_out=self.output_count(),
            outs=outs)


class RawTransaction(AbstractTransaction):
    
    def __init__(self, n_in, inputs, n_out, outputs):
        self._inputs = inputs
        self._outputs = outputs
        

class Transaction(AbstractTransaction):
    
    def __init__(self, inputs, outputs):
        self._inputs = inputs
        self._outputs = outputs
        self._block = None
        
        for input in inputs:
            input._set_txn(self)

        for idx, output in enumerate(outputs):
            output._set_txn(self, idx)
        
    def _set_block(self, block):
        self._block = block
        
    def block(self):
        return self._block
    
    
class NullTransaction(Transaction):
    
    def __init__(self):
        Transaction.__init__(self, [], [])
        
    def id(self):
        from .chain import Algochain
        return Algochain.NULL_ID