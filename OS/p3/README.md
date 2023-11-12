# README

sudo dd if=/dev/zero of=file bs=4096 count=10000

ls -l file

sudo ./cs238 --truncate file

sudo valgrind --leak-check=full ./cs238 --truncate file
