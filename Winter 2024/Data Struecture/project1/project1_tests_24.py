import requirements
import random as rand

# Instructions
# Some test cases for the CuckooHash24 class can be found below.
#
# Note that the test cases here are just to give an idea of how we will test your submissions, so passing these tests does not mean that your code is correct.
# It is a good idea to try and create different test cases with different table sizes to fully test your implementation.

def cuckoo_hash_tests():
	print("starting test 1")

	input_size, table_size = 10, 10
	nums = [i for i in range(input_size)]
	
	c = requirements.CuckooHash24(table_size)
	for num in nums:
		print("inserting %d" % num)
		no_cycle = c.insert(num)
		if no_cycle == False:
			print("error: cycle should not exist")

	for num in nums[:5]:
		print("deleting %d" % num)
		c.delete(num)
		if c.lookup(num):
			print("error: %d should not exist in cuckoo hash" %num)

	ref_ans = [[None, None, [7], None, [6], None, None, [5, 8], None, [9]], [None, None, None, None, None, None, None, None, None, None]]

	if not c.get_table_contents() == ref_ans:
		print("test 1 table contents incorrect")
		return
	print("\ntest 1 table contents correct")

	print("\n\nstarting test 2")

	input_size, table_size = 20, 10
	nums = [i for i in range(input_size*4)]

	c = requirements.CuckooHash24(table_size)
	for num in nums:
		# there should be a cycle when inserting 71
		print("inserting %d" % num)
		no_cycle = c.insert(num)
		if no_cycle == False:
			print("found cycle when inserting %d" %num)
			break

	ref_ans = [[[2, 11, 16, 44], [68, 39, 47, 53], [1, 7, 14, 30], [27, 36, 62, 55], [4, 6, 12, 49], [13, 18, 22, 69], [0, 10, 40, 35], [59, 8, 20, 23], [26, 28, 63, 43], [3, 54, 32, 41]], [[24], [37, 29, 67, 70], [25, 46, 45, 64], [15, 33, 56, 38], [5, 52, 48, 66], [21, 58, 60], [31, 34, 9, 61], [17, 42], [19, 51], [50, 57, 65]]]

	if not c.get_table_contents() == ref_ans:
		print("test 2 table contents incorrect")
		return
	
	c.rehash(20)

	ref_ans = [[[39, 47, 37, 38], [19], [68, 12, 49, 18], [62, 22, 21, 24], [4, 40, 20, 56], [44, 42], [36, 54, 32], [13], [48, 58, 34, 51], [16, 50, 55, 23], [45, 66, 60], [59, 33, 52], [2, 17, 1, 7], [61, 65], [6, 57], [11, 35, 26, 31], [27, 25], [30, 3, 5], [69, 0, 8], [10, 64, 9]], [[28], None, None, [14], None, [15], None, [46], None, None, None, [43, 70], None, [63], None, [29, 67], None, None, [41], [53]]]

	if not c.get_table_contents() == ref_ans:
		print("test 2 table contents incorrect")
		return
	print("\ntest 2 table contents correct")

if __name__ == '__main__':
	cuckoo_hash_tests()

