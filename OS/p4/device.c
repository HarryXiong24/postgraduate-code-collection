/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * device.c
 */

#define _GNU_SOURCE

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <unistd.h>
#include <fcntl.h>
#include "device.h"

/**
 * Needs:
 *   fstat()
 *   S_ISREG()
 *   S_ISBLK()
 *   ioctl()
 *   open()
 *   close()
 *   pread()
 *   pwrite()
 */

struct device {
	int fd;
	uint64_t size;  /* immutable */
	uint64_t block; /* immutable */
};

static int
geometry(struct device *device)
{
	struct stat st;
	uint64_t u64;
	uint32_t u32;

	u64 = u32 = 0;
	if (fstat(device->fd, &st)) {
		TRACE("fstat()");
		return -1;
	}
	if (S_ISREG(st.st_mode)) {
		u64 = st.st_size;
		u32 = st.st_blksize;
	}
	else if (S_ISBLK(st.st_mode)) {
		if (ioctl(device->fd, BLKGETSIZE64, &u64) ||
		    ioctl(device->fd, BLKSSZGET, &u32)) {
			TRACE("ioctl()");
			return -1;
		}
	}
	else {
		TRACE("device not supported");
		return -1;
	}
	if (!u32 || !u64) {
		TRACE("bad device geometry");
		return -1;
	}
	device->size = u64 / u32 * u32;
	device->block = u32;
	return 0;
}

struct device *
device_open(const char *pathname)
{
	struct device *device;

	assert( safe_strlen(pathname) );

	if (!(device = malloc(sizeof (struct device)))) {
		TRACE("out of memory");
		return NULL;
	}
	memset(device, 0, sizeof (struct device));
	if (0 >= (device->fd = open(pathname, O_RDWR | O_DIRECT))) {
		if (EACCES == errno) {
			device_close(device);
			TRACE("no volume access");
			return NULL;
		}
		device_close(device);
		TRACE("open()");
		return NULL;
	}
	if (geometry(device)) {
		device_close(device);
		TRACE(0);
		return NULL;
	}
	return device;
}

void
device_close(struct device *device)
{
	if (device) {
		if (0 < device->fd) {
			if (close(device->fd)) {
				TRACE("close()");
			}
		}
		memset(device, 0, sizeof (struct device));
	}
	FREE(device);
}

int
device_read(struct device *device, void *buf, uint64_t off, uint64_t len)
{
	assert( !len || buf );
	assert( 0 == (off % device->block) );
	assert( 0 == (len % device->block) );
	assert( (off + len) <= device->size );

	if (len != (uint64_t)pread(device->fd, buf, (size_t)len, (off_t)off)) {
		TRACE("pread()");
		return -1;
	}
	return 0;
}

int
device_write(struct device *device,
	     const void *buf,
	     uint64_t off,
	     uint64_t len)
{
	assert( !len || buf );
	assert( 0 == (off % device->block) );
	assert( 0 == (len % device->block) );
	assert( (off + len) <= device->size );

	if (len != (uint64_t)pwrite(device->fd,
				    buf,
				    (size_t)len,
				    (off_t)off)) {
		TRACE("pwrite()");
		return -1;
	}
	return 0;
}

uint64_t
device_size(const struct device *device)
{
	assert( device );

	return device->size;
}

uint64_t
device_block(const struct device *device)
{
	assert( device );

	return device->block;
}
