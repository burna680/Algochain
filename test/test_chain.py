import unittest

from algochain import chain

import resources


CHAIN_PATH = 'test/data/{}.txt'


class AlgochainTest(unittest.TestCase): 
    
    def _write_to_file(self, block):
        filename = '/tmp/algochain_test_tmp'
        with open(filename, 'w') as _file:
            _file.write(block.serialize())
        return filename
    
    def test_read_genesis(self):
        filename = CHAIN_PATH.format('genesis')
        algochain = chain.Algochain.from_file(filename)
        
        self.assertEqual(1, algochain.block_count())
        
        genesis = algochain.get_block(height=0)
        
        expected = '372e54f079effa865ec7b6422e2d2be3b37524a2d1d72b3bcb8a7e72f3e27098'
        self.assertEqual(expected, genesis.txns_hash())

        utxos = algochain.get_utxos('8bf7a0e432073e2d078d67dc4503936849b05d50566ecc8a803df1eed7706f82')
        self.assertEqual(1, len(utxos))
        self.assertEqual(0, utxos[0].idx_in_txn())

    def test_read_basic(self):
        filename = CHAIN_PATH.format('basic')
        algochain = chain.Algochain.from_file(filename)
        
        self.assertEqual(2, algochain.block_count())
        
        genesis = algochain.get_block(height=0)
        block = algochain.get_block(height=1)

        self.assertEqual(genesis.id(), block.previous().id())
        self.assertEqual(1, block.txn_count())

        txn = block.get_txn(0)
        
        self.assertEqual(1, txn.input_count())
        self.assertEqual(2, txn.output_count())
        
        prev_out = txn.get_input(0).prev_output()
        
        self.assertEqual(prev_out.txn().id(), genesis.get_txn(0).id())
        
        utxos = algochain.get_utxos('0618013fa64ac6807bdea212bbdd08ffc628dd440fa725b92a8b534a842f33e9')
        self.assertEqual(1, len(utxos))
        self.assertEqual(0, utxos[0].idx_in_txn())
        
        utxos = algochain.get_utxos('5855074c699e43bd8330d8280998ad872514d14f5b71d2b0c36bf9d39340c498')
        self.assertEqual(1, len(utxos))
        self.assertEqual(1, utxos[0].idx_in_txn())
        
        utxos = algochain.get_utxos('8bf7a0e432073e2d078d67dc4503936849b05d50566ecc8a803df1eed7706f82')
        self.assertEqual(0, len(utxos))
        
    def test_read_with_invalid_addr_on_input(self):
        filename = CHAIN_PATH.format('invalid_addr_on_input')
        
        self.assertRaises(
            chain.InvalidInputAddress,
            chain.Algochain.from_file,
            filename)  

    def test_read_with_invalid_input_sum(self):
        filename = CHAIN_PATH.format('invalid_input_sum')
        
        self.assertRaises(
            chain.NotEnoughFunds,
            chain.Algochain.from_file,
            filename)  

    def test_read_with_double_spending(self):
        filename = CHAIN_PATH.format('double_spending')
        
        self.assertRaises(
            chain.OutputAlreadySpent,
            chain.Algochain.from_file,
            filename)  