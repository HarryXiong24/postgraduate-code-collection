import requirements

# Instructions
# Some test cases for the Trie class can be found in the main block below.
#
# Note that passing the test cases here does not necessarily mean that your trie
# is correctly implemented / will pass other cases. It is a good idea to try and create different
# test cases.

def trie_tests():

	data = ["test", "toaster", "toasting", "slow", "slowly"]


	print('testing uncompressed trie')

	uncompressed_trie = requirements.Trie(is_compressed=False)
	uncompressed_trie.construct_trie_from_text(data)

	print(f'search_and_get_depth(test): {uncompressed_trie.search_and_get_depth("test")}, Expected: 4')
	print(f'search_and_get_depth(toaster): {uncompressed_trie.search_and_get_depth("toaster")}, Expected: 7')
	print(f'search_and_get_depth(toasting): {uncompressed_trie.search_and_get_depth("toasting")}, Expected: 8')
	print(f'search_and_get_depth(slow): {uncompressed_trie.search_and_get_depth("slow")}, Expected: 4')
	print(f'search_and_get_depth(slowly): {uncompressed_trie.search_and_get_depth("slowly")}, Expected: 6')
	print(f'search_and_get_depth(tempo): {uncompressed_trie.search_and_get_depth("tempo")}, Expected: -1')
	print(f'search_and_get_depth(team): {uncompressed_trie.search_and_get_depth("team")}, Expected: -1')
	print(f'search_and_get_depth(slowing): {uncompressed_trie.search_and_get_depth("slowing")}, Expected: -1\n')


	print('testing compressed trie')

	compressed_trie = requirements.Trie(is_compressed=True)
	compressed_trie.construct_trie_from_text(data)

	print(f'search_and_get_depth(test): {compressed_trie.search_and_get_depth("test")}, Expected: 2')
	print(f'search_and_get_depth(toaster): {compressed_trie.search_and_get_depth("toaster")}, Expected: 3')
	print(f'search_and_get_depth(toasting): {compressed_trie.search_and_get_depth("toasting")}, Expected: 3')
	print(f'search_and_get_depth(slow): {compressed_trie.search_and_get_depth("slow")}, Expected: 1')
	print(f'search_and_get_depth(slowly): {compressed_trie.search_and_get_depth("slowly")}, Expected: 2')
	print(f'search_and_get_depth(tempo): {uncompressed_trie.search_and_get_depth("tempo")}, Expected: -1')
	print(f'search_and_get_depth(team): {uncompressed_trie.search_and_get_depth("team")}, Expected: -1')
	print(f'search_and_get_depth(slowing): {uncompressed_trie.search_and_get_depth("slowing")}, Expected: -1\n')

	
	print('testing uncompressed suffix tree')

	uncompressed_suffix_tree = requirements.Trie(is_compressed=False)
	uncompressed_suffix_tree.construct_suffix_tree_from_text(data)

	print(f'search_and_get_depth(test): {uncompressed_suffix_tree.search_and_get_depth("test")}, Expected: 4')
	print(f'search_and_get_depth(toaster): {uncompressed_suffix_tree.search_and_get_depth("toaster")}, Expected: 7')
	print(f'search_and_get_depth(toasting): {uncompressed_suffix_tree.search_and_get_depth("toasting")}, Expected: 8')
	print(f'search_and_get_depth(slow): {uncompressed_suffix_tree.search_and_get_depth("slow")}, Expected: 4')
	print(f'search_and_get_depth(slowly): {uncompressed_suffix_tree.search_and_get_depth("slowly")}, Expected: 6')
	print(f'search_and_get_depth(tempo): {uncompressed_trie.search_and_get_depth("tempo")}, Expected: -1')
	print(f'search_and_get_depth(team): {uncompressed_trie.search_and_get_depth("team")}, Expected: -1')
	print(f'search_and_get_depth(slowing): {uncompressed_trie.search_and_get_depth("slowing")}, Expected: -1')
	print(f'search_and_get_depth(est): {uncompressed_suffix_tree.search_and_get_depth("est")}, Expected: 3')
	print(f'search_and_get_depth(ing): {uncompressed_suffix_tree.search_and_get_depth("ing")}, Expected: 3')
	print(f'search_and_get_depth(st): {uncompressed_suffix_tree.search_and_get_depth("st")}, Expected: 2\n')


	print('testing compressed suffix tree')

	compressed_suffix_tree = requirements.Trie(is_compressed=True)
	compressed_suffix_tree.construct_suffix_tree_from_text(data)

	print(f'search_and_get_depth(test): {compressed_suffix_tree.search_and_get_depth("test")}, Expected: 3')
	print(f'search_and_get_depth(toaster): {compressed_suffix_tree.search_and_get_depth("toaster")}, Expected: 3')
	print(f'search_and_get_depth(toasting): {compressed_suffix_tree.search_and_get_depth("toasting")}, Expected: 3')
	print(f'search_and_get_depth(slow): {compressed_suffix_tree.search_and_get_depth("slow")}, Expected: 2')
	print(f'search_and_get_depth(slowly): {compressed_suffix_tree.search_and_get_depth("slowly")}, Expected: 3')
	print(f'search_and_get_depth(tempo): {compressed_suffix_tree.search_and_get_depth("tempo")}, Expected: -1')
	print(f'search_and_get_depth(team): {compressed_suffix_tree.search_and_get_depth("team")}, Expected: -1')
	print(f'search_and_get_depth(slowing): {compressed_suffix_tree.search_and_get_depth("slowing")}, Expected: -1')
	print(f'search_and_get_depth(est): {compressed_suffix_tree.search_and_get_depth("est")}, Expected: 2')
	print(f'search_and_get_depth(ing): {compressed_suffix_tree.search_and_get_depth("ing")}, Expected: 1')
	print(f'search_and_get_depth(st): {compressed_suffix_tree.search_and_get_depth("st")}, Expected: 2\n')


	# add new tests...


if __name__ == '__main__':
	trie_tests()
