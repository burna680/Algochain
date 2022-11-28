import os
import unittest

from algochain.proto.client import Client
from algochain import chain


CHAIN_PATH = 'test/data/{}.txt'


class ProtocolTest(unittest.TestCase):
    
    def setUp(self):
        self._client = Client()
        
    def _get_balance(self, user):
        return self._client.balance(user).resource()        
    
    def test_init(self):
        output = self._client.init(user='user1', value=100, bits=5)
        chain = self._client.chain()
        
        self.assertTrue(output.ok())
        self.assertEqual(1, chain.block_count())
        self.assertEqual(100, self._client.balance('user1').resource())
        self.assertEqual(0, self._client.balance('user2').resource())
        
        block_id = output.resource()
        output = self._client.block(block_id)
        
        self.assertTrue(output.ok())
        
        prev_block = output.resource().previous()
        self.assertTrue(prev_block.is_null())
        
    def test_multiple_inits(self):
        output1 = self._client.init(user='user1', value=100, bits=5)
        output2 = self._client.init(user='user2', value=100, bits=5)
        chain = self._client.chain()
        
        self.assertTrue(output1.ok())
        self.assertTrue(output2.ok())
        
        block_id1 = output1.resource()
        block_id2 = output2.resource()
        
        self.assertNotEqual(block_id1, block_id2)
        self.assertEqual(1, chain.block_count())
        self.assertEqual(100, self._client.balance('user2').resource())
        self.assertEqual(0, self._client.balance('user1').resource())
        
    def test_load(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        chain = self._client.chain()
        
        self.assertTrue(output.ok())
        self.assertEqual(3, chain.block_count())
        self.assertEqual(5, self._client.balance('user1').resource())
        self.assertEqual(5, self._client.balance('user2').resource())
        self.assertEqual(90, self._client.balance('user3').resource())

    def test_multiple_loads(self):
        _ = self._client.load(CHAIN_PATH.format('basic'))
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        chain = self._client.chain()
        
        self.assertTrue(output.ok())
        self.assertEqual(3, chain.block_count())
        self.assertEqual(5, self._client.balance('user1').resource())
        self.assertEqual(5, self._client.balance('user2').resource())
        self.assertEqual(90, self._client.balance('user3').resource())
        
    def test_save(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())
        
        filename = CHAIN_PATH.format('temp_chain')
        output = self._client.save(filename)
        self.assertTrue(output.ok())
        
        self._client.init(user='user1', value=100, bits=5)
        output = self._client.load(filename)
        self.assertTrue(output.ok())
        
        chain = self._client.chain()
        self.assertEqual(3, chain.block_count())
        self.assertEqual(5, self._client.balance('user1').resource())
        self.assertEqual(5, self._client.balance('user2').resource())
        self.assertEqual(90, self._client.balance('user3').resource())
        
        os.remove(filename)
        
    def test_simple_transfers(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())      

        chain = self._client.chain()
        prev_tip = chain.tip()
   
        output1 = self._client.transfer(src='user3', dst={'user1':10, 'user4': 10})
        self.assertTrue(output1.ok())
        
        output2 = self._client.transfer(src='user2', dst={'user4':5})
        self.assertTrue(output2.ok())
        
        output3 = self._client.mine(bits=5)
        self.assertTrue(output3.ok())

        txn_id1 = output1.resource()
        txn1 = chain.get_txn(txn_id1)
        self.assertEqual(1, txn1.input_count())
        self.assertEqual(3, txn1.output_count())
        
        txn_id2 = output2.resource()
        txn2 = chain.get_txn(txn_id2)
        self.assertEqual(1, txn2.input_count())
        self.assertEqual(1, txn2.output_count())
        
        block_id = output3.resource()
        block = chain.get_block(id=block_id)
        self.assertEqual(prev_tip.id(), block.previous().id())
        
        self.assertEqual(4, chain.block_count())
        self.assertEqual(15, self._client.balance('user1').resource())
        self.assertEqual(0, self._client.balance('user2').resource())
        self.assertEqual(70, self._client.balance('user3').resource())
        self.assertEqual(15, self._client.balance('user4').resource())
        
    def test_chained_transfers_in_same_block(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())      

        chain = self._client.chain()
        prev_tip = chain.tip()
        
        self.assertEqual(0, self._client.balance('user5').resource())
        
        output1 = self._client.transfer(src='user3', dst={'user1':10, 'user4': 10})
        output2 = self._client.transfer(src='user1', dst={'user4':10})
        output3 = self._client.transfer(src='user4', dst={'user5':10, 'user6': 9})
        output4 = self._client.mine(bits=5)
        
        self.assertTrue(output1.ok())
        self.assertTrue(output2.ok())
        self.assertTrue(output3.ok())
        self.assertTrue(output4.ok())
        
        txn_id = output3.resource()
        txn = chain.get_txn(txn_id)
        self.assertEqual(2, txn.input_count())
        self.assertEqual(3, txn.output_count())
        
        block_id = output4.resource()
        block = chain.get_block(id=block_id)
        self.assertEqual(prev_tip.id(), block.previous().id())
        
        self.assertEqual(4, chain.block_count())
        self.assertEqual(5, self._client.balance('user1').resource())
        self.assertEqual(5, self._client.balance('user2').resource())
        self.assertEqual(70, self._client.balance('user3').resource())
        self.assertEqual(1, self._client.balance('user4').resource())
        self.assertEqual(10, self._client.balance('user5').resource())    
        self.assertEqual(9, self._client.balance('user6').resource())
        
    def test_balance_on_unconfirmed_transactions(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())

        output1 = self._client.transfer(src='user3', dst={'user1':10, 'user4': 10})
        self.assertTrue(output1.ok())
        
        balance1 = self._get_balance('user1')
        balance2 = self._get_balance('user2')
        balance3 = self._get_balance('user3')
        balance4 = self._get_balance('user4')
        balance5 = self._get_balance('user5')
        
        self.assertEqual(15, balance1)
        self.assertEqual(5, balance2)
        self.assertEqual(70, balance3)
        self.assertEqual(10, balance4)
        self.assertEqual(0, balance5)
        
        output2 = self._client.mine(bits=5)
        self.assertTrue(output2.ok())
        
        balance1 = self._get_balance('user1')
        balance2 = self._get_balance('user2')
        balance3 = self._get_balance('user3')
        balance4 = self._get_balance('user4')
        balance5 = self._get_balance('user5')
        
        self.assertEqual(15, balance1)
        self.assertEqual(5, balance2)
        self.assertEqual(70, balance3)
        self.assertEqual(10, balance4)
        self.assertEqual(0, balance5)        
        
    def test_transfer_null_value(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())      

        output = self._client.transfer(src='user3', dst={'user1':0, 'user4': 0, 'user5':0})
        self.assertFalse(output.ok())
        self.assertEqual(chain.InvalidTransactionAmount().message, output.reason())

    def test_transfer_negative_value(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())      

        output = self._client.transfer(src='user3', dst={'user1':10, 'user4': -1, 'user5':4})
        self.assertFalse(output.ok())
        self.assertEqual(chain.InvalidTransactionAmount().message, output.reason())
        
    def test_insufficient_funds_on_transfer(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())      

        output = self._client.transfer(src='user3', dst={'user1':80, 'user4': 10, 'user5':0.5})
        self.assertFalse(output.ok())
        self.assertEqual(chain.NotEnoughFunds().message, output.reason())
        
    def test_insufficient_funds_after_several_transfers(self):
        output = self._client.load(CHAIN_PATH.format('blocks=3_user1=5_user2=5_user3=90'))
        self.assertTrue(output.ok())      

        output = self._client.transfer(src='user3', dst={'user1':80, 'user4': 10})
        self.assertTrue(output.ok())
        
        output = self._client.transfer(src='user3', dst={'user1':1})
        self.assertFalse(output.ok())
        self.assertEqual(chain.NotEnoughFunds().message, output.reason())