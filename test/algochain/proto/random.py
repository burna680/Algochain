import collections
import random
import string

from algochain.hash import Hasher
from .client import Client


vowels = 'aeiou'
consonants = list(
                filter(lambda char: char not in vowels + 'yq',
                       string.ascii_lowercase)
            )


class RandomGenerator(object):
    
    def __init__(self):
        self._client = Client()
        self._users = dict()
        self._blocks = list()
        self._txns = list()
        self._cmds = list()
        
    def generate(self, n=None):
        n = n or random.randint(10,1000)
        self._init()
        for _ in range(n):
            self._rand_cmd()
        return self.chain()
            
    def commands(self):
        return self._cmds
    
    def chain(self):
        return self._client.chain()
    
    def balances(self):
        return {usr:val for usr,val in self._users.items() if val != 0}
    
    def _init(self):
        satoshi = self._random_username()
        balance = self._rand_value()
        bits = self._rand_bits()
        
        output = self._client.init(satoshi, balance, bits=bits)
        self._users[satoshi] = balance
        self._cmds.append(f'init {satoshi} {balance} {bits}')
        self._blocks.append(output.resource())
        
    def _balance(self):
        if random.random() < 0.5:
            user = random.choice(list(self._users.keys()))
        else:
            user = self._random_username()
            
        output = self._client.balance(user)
        self._cmds.append(f'balance {user}')
        if output.resource() != self._users.get(user,0):
            print(f'WARNING: bad balance for {user} (expected {self._users.get(user,0)}, got {output.resource()})')
    
    def _transfer(self):
        if random.random() < 0.8:
            k = 100
            while k > 0:
                source = random.choice(list(self._users.keys()))
                if self._users.get(source,0) > 0:
                    break
                k -= 1
            if k == 0:
                source = self._random_username()   
        else:
            source = self._random_username()
        balance = self._users.get(source,0)
        
        if random.random() < 0.8 or balance == 0:
            amount = random.randint(0,balance)
            valid = True
        else:
            amount = random.randint(balance+10, balance*1000)
            valid = False
            
        targets = dict()
        
        if amount == 0:
            targets[source] = 0
        
        remaining = amount
        while remaining > 0:
            if random.random() < 0.5:
                user = random.choice(list(self._users.keys()))
            else:
                user = self._random_username()
            value = random.randint(0,remaining)
            if user in targets:
                targets[user] += value
            else:
                targets[user] = value
            remaining -= value
            
        if len(targets) == 1 and source in targets and targets[source] == 0:
            valid = False
            
        output = self._client.transfer(source, targets)
            
        dsts = ' '.join([f'{usr} {val}' for usr,val in targets.items()])
        cmd_str = f'transfer {source} {dsts}'
        self._cmds.append(cmd_str)
        
        if valid:
            self._txns.append(output.resource())
            self._users[source] = balance - amount
            for usr,val in targets.items():
                if usr in self._users:
                    self._users[usr] += val
                else:
                    self._users[usr] = val
    
    def _mine(self):
        bits = self._rand_bits()
        
        output = self._client.mine(bits)
        self._cmds.append(f'mine {bits}')
        self._blocks.append(output.resource())
    
    def _block(self):
        if random.random() < 0.5 and self._blocks:
            block_id = random.choice(self._blocks)
        else:
            user = self._random_username()
            block_id = Hasher.hash(user)
            
        output = self._client.block(block_id)
        self._cmds.append(f'block {block_id}')
    
    def _transaction(self):
        if random.random() < 0.5 and self._txns:
            txid = random.choice(self._txns)
        else:
            user = self._random_username()
            txid = Hasher.hash(user)
            
        output = self._client.transaction(txid)
        self._cmds.append(f'transaction {txid}')   
        
    def _rand_cmd(self):
        cmd = random.choice(['balance', 'transfer', 'mine', 'block', 'transaction'])
        if cmd == 'balance':
            self._balance()
        elif cmd == 'transfer':
            self._transfer()
        elif cmd == 'mine':
            self._mine()
        elif cmd == 'block':
            self._block()
        elif cmd == 'transaction':
            self._transaction()
        
    def _random_username(self):
        def method1(sz):
            return ''.join([random.choice(vowels)
                                if i%3
                                else random.choice(consonants)
                            for i in range(sz)])
                            
        def method2(sz):
            return ''.join([random.choice(vowels)
                                if i%3 == 1
                                else random.choice(consonants)
                            for i in range(sz)])
                            
        def method3(sz):
            return ''.join([
                            random.choice(consonants),
                            random.choice(vowels),
                            random.choice(vowels),
                            random.choice(consonants),
                            random.choice(consonants),
                            random.choice(vowels)])    
        
        size = random.randint(3, 7)
        method = random.choice([method1, method2, method3])
        
        return method(size)
    
    def _rand_bits(self):
        return random.randint(1,12)
    
    def _rand_value(self):
        return random.randint(1,1000000)    