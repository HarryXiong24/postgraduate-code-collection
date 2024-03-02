# explanations for member functions are provided in requirements.py
# each file that uses a skip list should import it from this file.

from typing import TypeVar
import random
from zip_tree import ZipTree

KeyType = TypeVar('KeyType')
ValType = TypeVar('ValType')

class SkipList:
	def __init__(self):
		pass

	def get_random_level(self, key: KeyType) -> int:
	  	# Do not change this function. Use this function to determine what level each key should be at. Assume levels start at 0 (i.e. the bottom-most list is at level 0)
		# e.g. for some key x, if get_random_level(x) = 5, then x should be in the lists on levels 0, 1, 2, 3, 4 and 5 in the skip list.
		random.seed(str(key))
		level = 0
		while random.random() < 0.5 and level < 20:
			level += 1
		return level

	def insert(self, key: KeyType, val: ValType):
		pass

	def remove(self, key: KeyType):
		pass

	def find(self, key: KeyType) -> ValType:
		pass

	def get_list_size_at_level(self, level: int):
		pass
		
	def from_zip_tree(self, zip_tree: ZipTree) -> None:
		pass
			

# feel free to define new classes/methods in addition to the above
# fill in the definitions of each required member function (above),
# and any additional member functions you define
