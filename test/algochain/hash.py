import hashlib


class Hasher(object):
    
    @classmethod
    def hash(cls, value):
        digest = cls._hash(value)
        return cls._hash(digest)
    
    @classmethod
    def _hash(cls, value):
        value_str = str(value)
        sha256 = hashlib.sha256()
        sha256.update(str.encode(value_str))
        return sha256.hexdigest()
        
        
class CollectionHash(object):
    
    def __init__(self, objs):
        self._objs = objs
        
    def value(self):
        raise NotImplementedError
    
    
class SequentialHash(CollectionHash):
    
    def value(self):
        string = str().join(map(str, self._objs))
        return Hasher.hash(string)
        
        
class MerkleTreeHash(CollectionHash):
    
    def value(self):
        current_level = list(map(str, self._objs))
        n = len(current_level)
        
        if n == 0:
            return Hasher.hash(str())
        
        while n > 1:
            new_level = list()
            
            if n & 1:
                current_level.append(current_level[-1])
            
            for i in range(0, n, 2):
                hash1 = Hasher.hash(current_level[i])
                hash2 = Hasher.hash(current_level[i+1])
                new_level.append(hash1 + hash2)
                
            current_level = new_level
            n = len(current_level)
        
        return Hasher.hash(current_level[0])
        