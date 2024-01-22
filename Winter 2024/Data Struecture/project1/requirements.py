from cuckoo_hash import CuckooHash
from cuckoo_hash_24 import CuckooHash24

# Please read all of the following before starting your implementation:
#
# Details about Gradescope submission:
#
# - You should not include anything outside of standard Python libraries.
# - Functions should be tested using Python 3.6+ on a Linux environment.
# - You must submit the requirements.py, cuckoo_hash.py, and cuckoo_hash_24.py files, along with any additional source files that you might create
# - The submission should either be the files themselves, or a zip file not containing any directories.
# - We have provided a project1_tests.py file that contains some simple test cases to give an idea of how we will be running your
#   code. Please use that file when testing your implementation.
#
#
# We have also provided the hash function, hash_func(), that you need to use when accessing table entries. Please do not define
# your own hash functions, as otherwise we will not able to test the correctness of your code. This hash function
# automatically updates itself when the rehash() method is called, so you can keep using this same function after
# any number of rehash() calls.
# 
#
# Explanations for CuckooHash public member functions
# Note: you may assume that each function will be called with valid values during testing
#
# init(): initializes the cuckoo hash tables as a list with dimensions 2 by init_sizes, and fills both tables with None entries.
#					CYCLE_THRESHOLD denotes the threshold for the number of evictions for detecting cycles in the insert() method.
#					do *not* change the value of this variable.
# 				
# get_table_contents(): will be used during testing to check the correctness of the table contents.
#
# hash_func(): takes as input the key and the table id (0 or 1), and returns the hash value for that key in the specified table.
#
#
# Methods that need to be completed:
#
# insert(key): insert an item with the specified key to the cuckoo hash. if a cycle is found during the insertion, stop and 
#							 return False. otherwise return True after inserting the item. if a cycle is found, do *not* rehash afterwards; only return False. to determine if there 
#							 is a cycle, use the provided CYCLE_THRESHOLD value, i.e. if the number of evictions is greater than CYCLE_THRESHOLD, you may assume there is a cycle.
#
# lookup(key): return True if an item with the specified key exists in the cuckoo hash, and False otherwise.
#
# delete(key): delete item with the specified key from the cuckoo hash and replace it with a None entry.
#
# rehash(new_table_size): update self.tables such that both tables are of size new_table_size, and all existing elements
#													in the old tables are rehashed to their new locations.
#
# cuckoo_hash_24.py:
#
# In common use, the hash buckets are multi-way set
# associative, i.e., each bucket of the cuckoo hash consist of multiple slots for items. 
# A bucket size of 4 is a common value in practice.
#
# Hint: You can restructure the cuckoo hash to implement the multi-way set or you can implement 
# the set associativity without changing the underlying tables. 
# 
# A lookup for key proceeds by computing two hashes of key to find buckets b1 and b2
# that could be used to store the key, and examining all of the
# slots within each of those buckets to determine if the key is
# present. You will modify your cuckoo hash to be a basic “2,4-cuckoo” hash table (two hash functions,
# four slots per bucket).
# A consequence of this is that Lookup operations always check 2B keys (B is the size of each bucket).
# To Insert a new key into the table, if either of the two
# buckets has an empty slot, it is then inserted in that bucket; if
# neither bucket has space, a random key from one candidate
# bucket is displaced by the new item. The displaced item is
# then relocated to its own alternate location, possibly displacing another item, and so on, until a maximum number of
# displacements is reached. You will use the same method for determining when to rehash.
#
# Copy the project1_tests.py to project1_tests_24.py and make modifications to test your 2,4-cuckoo
 
