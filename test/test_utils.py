import os
import unittest

from algochain import utils
from algochain.chain import Validator

import resources


CHAIN_PATH = 'test/data/{}.txt'


class UtilsTest(unittest.TestCase): 
    
    def _write_to_file(self, block):
        filename = '/tmp/algochain_test_tmp'
        with open(filename, 'w') as _file:
            _file.write(block.serialize())
        return filename
    
    def test_leading_zeros(self):
        self.assertEqual(4, Validator.count_leading_zeros('0f234'))
        self.assertEqual(0, Validator.count_leading_zeros('ff234'))
        self.assertEqual(10, Validator.count_leading_zeros('00234'))
        self.assertEqual(1, Validator.count_leading_zeros('7f234'))
    
    def test_valid_block(self):
        block = resources.basic_raw_block()
        self.assertTrue(utils.is_valid_block(block))
        
    def test_read_block_from_file(self):
        block = resources.basic_raw_block()
        filename = self._write_to_file(block)
        
        parsed_block = utils._read_block(filename)

        self.assertEqual(block.serialize(), parsed_block.serialize())
        
    def test_valid_block_from_file(self):    
        block = resources.basic_raw_block()
        filename = self._write_to_file(block)
        
        try:
            is_valid = utils.is_valid_block(filename)
        except Exception:
            is_valid = False
        finally:
            os.remove(filename)
            
        self.assertTrue(is_valid)
    
    def test_invalid_block(self):
        block = resources.basic_raw_block()
        block._header._nonce = 0
        self.assertFalse(utils.is_valid_block(block))
    
    def test_invalid_txns_hash_in_block(self):
        block = resources.basic_raw_block()
        block._txns[0]._inputs[0]._addr = 'asdffdsa'
        self.assertFalse(utils.is_valid_block(block))
        
    def test_build_block(self):
        txn = resources.basic_raw_txn()
        block = utils.build_block([txn], bits=3)
        self.assertTrue(utils.is_valid_block(block))