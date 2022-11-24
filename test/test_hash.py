import unittest

from algochain.hash import Hasher, MerkleTreeHash

import resources


class HashTest(unittest.TestCase):
    
    def test_hash_raw_transaction(self):
        txn = resources.basic_raw_txn()
        
        expected = '155dc94b29dce95bb2f940cdd2d7e0bce66dca9370c3ed96d50a30b3d84f8c4c'
        
        self.assertEqual(expected, txn.id())
        
    def test_hash_raw_block(self):
        block = resources.basic_raw_block()
        
        expected = '94a2c13fc51125d57bb7cdcfb8e67b7987fa19cbac88593df1592ab28432afca'
        
        self.assertEqual(expected, block.id())
        
    def test_hash_raw_block_header(self):
        block = resources.basic_raw_block()
        
        expected = '08850af2009f750a59dd91d0d5d4bafa25c6421de132336e2c5ee67bba6e352e'
        hdr_hash = Hasher.hash(block.header().serialize())
        
        self.assertEqual(expected, hdr_hash)
        
    def test_merkle_hash(self):
        objs = ['asdf']
        _hash = MerkleTreeHash(objs).value()
        self.assertEqual('7624e1f89ce009f8ec7e6e39781a42c0a27fa38f94db4f05f78b0f301007e06a', _hash)

        objs = ['asdf', 'fdsa']
        _hash = MerkleTreeHash(objs).value()
        self.assertEqual('d82966e8b122f4c1f242ed5626b1be57586ec30c9a3a5ab3708eb02983a5ebd5', _hash)

        objs = ['asdf', 'fdsa', 'asdf']
        _hash = MerkleTreeHash(objs).value()
        self.assertEqual('d4893c69ef8789a5df8632145873dfa0120580c98f8301b6c597d433f2133a07', _hash)

        objs = ['asdf', 'fdsa', 'asdf', 'fdsa']
        _hash = MerkleTreeHash(objs).value()
        self.assertEqual('8ba0b563b64e4b9948947a21583a30fd350294b8cdf244f72875f30cda71b24d', _hash)

        objs = ['asdf', 'fdsa', 'asdf', 'fdsa', 'yeah', 'yeah']
        _hash = MerkleTreeHash(objs).value()
        self.assertEqual('4741d32d1af86630a5e0e51233ead166edc7ed367864bc39621fb44a279bbf70', _hash)
        
        _hash = MerkleTreeHash([]).value()
        self.assertEqual('cd372fb85148700fa88095e3492d3f9f5beb43e555e5ff26d95f5a6adc36f8e6', _hash)
        
        