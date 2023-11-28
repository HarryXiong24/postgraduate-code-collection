# README

gcloud compute scp --recurse "/Users/harryxiong24/Code/Study/grad-code-collection/OS/p4" ubuntu:

## Creating file before run

sudo dd if=/dev/zero of=file bs=4096 count=10000

## Create device

sudo mkfs.ext4 file
sudo mknod -m0660 /dev/loop24 b 7 11
sudo losetup /dev/loop24 file

## Check

lsblk

## Exec

sudo ./cs238 /dev/loop24

## For check

stat -f /dev/loop24

## Uninstall

sudo umount /dev/loop24
sudo losetup -d /dev/loop24

## Extra Credit

dd if=/dev/zero of=fileEx bs=4096 count=100
sudo ./cs238 /dev/loop24 fileEx
