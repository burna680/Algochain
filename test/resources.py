from algochain.txn import RawTransaction, RawInput, RawOutput
from algochain.block import RawBlock


def basic_raw_txn():
    txid = '26429a356b1d25b7d57c0f9a6d5fed8a290cb42374185887dcd2874548df0779'
    idx = 2
    addr = 'f680e0021dcaf15d161604378236937225eeecae85cc6cda09ea85fad4cc51bb'
    _input = RawInput(txid, idx, addr)
    
    value = 250.5
    addr = '0618013fa64ac6807bdea212bbdd08ffc628dd440fa725b92a8b534a842f33e9'
    _output = RawOutput(value, addr)
    
    return RawTransaction(1, [_input], 1, [_output])

def basic_raw_block():
    txn = basic_raw_txn()
    prev_block = 'ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff'
    txns_hash = '155dc94b29dce95bb2f940cdd2d7e0bce66dca9370c3ed96d50a30b3d84f8c4c'
    bits = 3
    nonce = 12238
    txn_count = 1
    
    return RawBlock(prev_block, txns_hash, bits, nonce, txn_count, [txn])