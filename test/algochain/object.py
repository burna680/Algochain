from .hash import Hasher


class AlgochainObject(object):
    
    def id(self):
        return Hasher.hash(self.serialize())
    
    def serialize(self):
        raise NotImplementedError
    
    def __str__(self):
        return self.serialize()
    
    def __repr__(self):
        return self.serialize()