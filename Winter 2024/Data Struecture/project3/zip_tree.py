# explanations for member functions are provided in requirements.py
# each file that uses a Zip Tree should import it from this file.

from typing import TypeVar

KeyType = TypeVar('KeyType')
ValType = TypeVar('ValType')

class ZipTree:
	def __init__(self):
		pass

	@staticmethod
	def get_random_rank() -> int:
		pass

	def insert(self, key: KeyType, val: ValType, rank: int = -1):
		pass

	def remove(self, key: KeyType):
		pass

	def find(self, key: KeyType) -> ValType:
		pass

	def get_size(self) -> int:
		pass

	def get_height(self) -> int:
		pass

	def get_depth(self, key: KeyType):
		pass

# feel free to define new classes/methods in addition to the above
# fill in the definitions of each required member function (above),
# and any additional member functions you define
