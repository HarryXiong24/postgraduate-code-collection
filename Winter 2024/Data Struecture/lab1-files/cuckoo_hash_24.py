# explanations for member functions are provided in requirements.py
# each file that uses a cuckoo hash should import it from this file.
import random as rand
from typing import List

class CuckooHash:
  # init(): initializes the cuckoo hash tables as a list with dimensions 2 by init_sizes, and fills both tables with None entries.
	def __init__(self, init_size: int):
		self.__num_rehashes = 0
		self.CYCLE_THRESHOLD = 10 # CYCLE_THRESHOLD denotes the threshold for the number of evictions for detecting cycles in the insert() method.

		self.table_size = init_size
		self.tables = [[None]*init_size for _ in range(2)]

  # hash_func(): takes as input the key and the table id (0 or 1), and returns the hash value for that key in the specified table.
	def hash_func(self, key: int, table_id: int) -> int:
		key = int(str(key) + str(self.__num_rehashes) + str(table_id))
		rand.seed(key)
		return rand.randint(0, self.table_size-1)

  # get_table_contents(): will be used during testing to check the correctness of the table contents.
	def get_table_contents(self) -> List[List[int]]:
		return self.tables

	# you should *NOT* change any of the existing code above this line
	# you may however define additional instance variables inside the __init__ method.

	def insert(self, key: int) -> bool:
		# TODO
		pass

	def lookup(self, key: int) -> bool:
		# TODO
		pass
		

	def delete(self, key: int) -> None:
		# TODO
		pass

  # rehash(new_table_size): update self.tables such that both tables are of size new_table_size, and all existing elements in the old tables are rehashed to their new locations.
	def rehash(self, new_table_size: int) -> None:
		self.__num_rehashes += 1; self.table_size = new_table_size # do not modify this line
		# TODO
		pass

	# feel free to define new methods in addition to the above
	# fill in the definitions of each required member function (above),
	# and for any additional member functions you define

