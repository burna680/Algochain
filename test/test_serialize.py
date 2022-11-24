import unittest

import resources


class SerializationTest(unittest.TestCase): 
    
    def test_raw_txn_serialization(self):
        txn = resources.basic_raw_txn()
        
        expected = (
            '1\n'
            '26429a356b1d25b7d57c0f9a6d5fed8a290cb42374185887dcd2874548df0779 2 f680e0021dcaf15d161604378236937225eeecae85cc6cda09ea85fad4cc51bb\n'
            '1\n'
            '250.5 0618013fa64ac6807bdea212bbdd08ffc628dd440fa725b92a8b534a842f33e9\n'    
        )
        
        serialized = txn.serialize()
        
        self.assertEqual(expected, serialized)
        
    def test_raw_block_serialization(self):
        block = resources.basic_raw_block()
        
        expected = (
            'ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff\n'
            '155dc94b29dce95bb2f940cdd2d7e0bce66dca9370c3ed96d50a30b3d84f8c4c\n'
            '3\n'
            '12238\n'
            '1\n'
            '1\n'
            '26429a356b1d25b7d57c0f9a6d5fed8a290cb42374185887dcd2874548df0779 2 f680e0021dcaf15d161604378236937225eeecae85cc6cda09ea85fad4cc51bb\n'
            '1\n'
            '250.5 0618013fa64ac6807bdea212bbdd08ffc628dd440fa725b92a8b534a842f33e9\n' 
        )
        
        serialized = block.serialize()
        
        self.assertEqual(expected, serialized)        
        
