from zip_tree import ZipTree
from skip_list import SkipList

# Details about Gradescope submission:

# - No file should include anything outside of standard Python libraries.
# - Functions should be tested using Python 3.6+ on a Linux environment.
# - The submission should either be the files themselves, or a zip file not containing any directories.


# Explanations for ZipTree public member functions

# any variable annotated with KeyType should use the same type for each tree, and should be comparable.
# ValType is for any additional data to be stored in the nodes.
# get_random_rank(): returns a random node rank, chosen independently from a geometric distribution of mean 1.
# insert(): inserts item with parameter key, value, and rank into tree.
#           if rank is not provided, a random rank should be selected by using ZipTree.get_random_rank().
# remove(): removes item with parameter key from tree.
#           you can assume that the item exists in the tree.
# find(): returns the value of item with parameter key.
#         you can assume that the item exists in the tree.
# get_size(): returns the number of nodes in the tree.
# get_height(): returns the height of the tree.
# get_depth(): returns the depth of the item with parameter key.
#              you can assume that the item exists in the tree.

# Explanations for SkipList public member functions

# KeyType and ValType are used the same way as in Zip Trees.
# get_random_level(): Use this function to determine what level each key should be at. Assume levels start at 0 (i.e. the bottom-most list is at level 0)
#											e.g. for some key x, if get_random_level(x) = 5, then x should be in the lists on levels 0, 1, 2, 3, 4 and 5 in the skip list.
# insert(): inserts item with parameter key and value into the skip list.
# remove(): removes item with parameter key from the skip list.
#						you can assume that the item exists in the skip list.
# find(): returns the value of item with parameter key.
#         you can assume that the item exists in the skip list.
# get_list_size_at_level(): returns number of skip list nodes at a given level. Assume levels start at 0 (i.e. the bottom-most list is at level 0).
#														if the parameter level exceeds the current maximum level in the skip list, return 0.
# from_zip_tree(): construct a skip list from the given zip tree as described in the paper (https://arxiv.org/pdf/1806.06726.pdf, "the level-k sublist comprises nodes.
#				   of the zip tree of rank k or less"). E.g. a node with rank 3 in the zip tree would be in the lists on levels 0, 1, 2 and 3 in the skip list.
#				   The constructed skip list nodes should have the same key/value information as they do in their corresponding nodes in the zip tree.
#				   You can assume that this function will only be called immediately after creating a skip list instance (as in the project3_tests.py file)

