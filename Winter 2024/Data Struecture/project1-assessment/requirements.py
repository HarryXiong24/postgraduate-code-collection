from cuckoo_hash_assess import CuckooHash24

# Please read all of the following before starting your implementation:
#
# Details about Gradescope submission:
#
# - You should not include anything outside of standard Python libraries.
# - Functions should be tested using Python 3.6+ on a Linux environment.
# - You must submit the requirements.py and the cuckoo_hash_assess.py files, along with any additional source files that you might create
# - The submission should either be the files themselves, or a zip file not containing any directories.
# - We have provided a project1_assessment_tests.py file that contains some simple test cases to give an idea of how we will be running your
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
# init(): initializes the cuckoo hash table as a list of size init_sizes, and fills it with None entries.
#					CYCLE_THRESHOLD denotes the threshold for the number of evictions for detecting cycles in the insert() method.
#					do *not* change the value of this variable.
# 				
# get_table_contents(): will be used during testing to check the correctness of the table contents.
#
# hash_func(): takes as input the key and the function id (0 or 1), and returns the hash value for that key
#
# Methods that need to be completed:
#
# insert(key): as described in the assessment announcement in Ed.
#
# lookup(key): return True if an item with the specified key exists in the cuckoo hash, and False otherwise.
#
# delete(key): delete item with the specified key from the cuckoo hash and replace it with a None entry.
#
# rehash(new_table_size): update self.table such that it is of size new_table_size, and all existing elements
#													in the old table are rehashed to their new locations.
#
