# README

gcloud compute scp --recurse "/Users/harryxiong24/Code/Study/grad-code-collection/OS/p4" ubuntu:

## Creating file before run

sudo dd if=/dev/zero of=file bs=4096 count=10000

## Create device

sudo mkfs.ext4 file
sudo mknod -m0660 /dev/loop11 b 7 11
sudo losetup /dev/loop11 file

## Check

lsblk

sudo losetup -a

## Exec

sudo ./cs238 /dev/loop11
sudo valgrind --leak-check=full ./cs238 /dev/loop11

## For check

stat -f /dev/loop11

## Uninstall

sudo umount /dev/loop11
sudo losetup -d /dev/loop11

## Extra Credit

dd if=/dev/zero of=fileEx bs=4096 count=100
sudo ./cs238 /dev/loop11 fileEx
