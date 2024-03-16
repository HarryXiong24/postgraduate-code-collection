from trie import Trie

# Details about Gradescope submission:

# - No file should include anything outside of standard Python libraries.
# - Functions should be tested using Python 3.6+ on a Linux environment.
# - The submission should either be the files themselves, or a zip file not containing any directories.


# Explanations for Trie public member functions

# The __init__ method for Trie comes with an is_compressed parameter that determines whether you will construct an uncompressed or compressed Trie/Suffix tree.
# construct_trie_from_text(): constructs a trie from the given list of strings. You can assume this method will be called exactly once after creating a Trie instance. 
#                             If the instance variable is_compressed is set to True, construct a compressed trie, otherwise construct an uncompressed trie.
# construct_suffix_tree_from_text(): constructs a suffix tree from the given list of strings. You can assume this method will be called exactly once after creating a Trie instance.
#                             If the instance variable is_compressed is set to True, construct a compressed suffix tree, otherwise construct an uncompressed suffix tree.
# search_and_get_depth(): given a key (i.e. a string), searches the trie/suffix tree for the key and returns the depth of the key in the data structure. If the key does not exist in the trie, return -1 instead. Note that the root node of the trie/suffix tree is at depth 0.
# note: when constructing tries/suffix trees, do not use a separate "end of string node" (the "$" nodes in the lecture notes). Instead, you can store this information in the last character node for each string.
#       for example, for an uncompressed trie that has the word "ten" inserted, search_and_get_depth("ten") should return 3 (not 4), and the path taken would be root -> t -> e -> n.

# you can assume the text given to the construct methods is lowercase.