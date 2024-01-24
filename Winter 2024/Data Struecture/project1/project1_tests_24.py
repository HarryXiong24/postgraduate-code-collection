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
		# there should be a cycle when inserting 75
		print("inserting %d" % num)
		no_cycle = c.insert(num)
		if no_cycle == False:
			print("found cycle when inserting %d" %num)
			break
 
	print(c.get_table_contents())

	ref_ans = [[[44, 2, 16, 25], [47, 39, 67, 68], [74, 7, 60, 30], [64, 27, 75, 55], [4, 12, 65, 21], [13, 70, 69, 38], [66, 10, 35, 61], [31, 8, 20, 48], [45, 28, 51, 63], [52, 9, 54, 41]], [[26, 24, 72], [18, 37, 29, 36], [0, 22, 46, 59], [15, 56, 33, 6], [5, 23, 71], [58, 43, 11, 73], [3, 34, 62, 49], [14, 53, 42, 17], [19], [32, 50, 57, 1]]]

	if not c.get_table_contents() == ref_ans:
		print("test 2 table contents incorrect")
		return

	c.rehash(20)

	ref_ans = [[[47, 39, 29, 37], [19], [68, 12, 18, 49], [21, 62, 63, 24], [4, 20, 56], [44, 71, 73, 42], [75, 54, 36, 32], [13], [48, 51, 58, 34], [14, 67, 43, 50], [60, 66, 45], [52, 59, 33], [1, 7, 17, 41], [74, 65, 61], [6, 57], [35, 31, 26, 11], [25, 27], [30, 5, 3], [69, 8, 72, 0], [64, 10, 9]], [[28, 55], None, None, None, [23], [15], None, [46], None, [16], None, [70], None, [38], None, None, [22], None, [2], [53]]]

	if not c.get_table_contents() == ref_ans:
		print("test 2 table contents incorrect")
		return
	print("\ntest 2 table contents correct")

if __name__ == '__main__':
	cuckoo_hash_tests()

