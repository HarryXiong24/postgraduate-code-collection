/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * logfs.h
 */

#ifndef _LOGFS_H_
#define _LOGFS_H_

#include "system.h"

struct logfs;

/**
 * Opens the block device specified in pathname for buffered I/O using an
 * append only log structure.
 *
 * pathname: the pathname of the block device
 *
 * return: an opaque handle or NULL on error
 */

struct logfs *logfs_open(const char *pathname);

/**
 * Closes a previously opened logfs handle.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 *
 * Note: logfs may be NULL.
 */

void logfs_close(struct logfs *logfs);

/**
 * Random read of len bytes at location specified in off from the logfs.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 * buf  : a region of memory large enough to receive len bytes
 * off  : the starting byte offset
 * len  : the number of bytes to read
 *
 * return: 0 on success, otherwise error
 */

int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len);

/**
 * Append len bytes to the logfs.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 * buf  : a region of memory holding the len bytes to be written
 * len  : the number of bytes to write
 *
 * return: 0 on success, otherwise error
 */

int logfs_append(struct logfs *logfs, const void *buf, uint64_t len);

#endif /* _LOGFS_H_ */
