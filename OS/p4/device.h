/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * device.h
 */

#ifndef _P_DEVICE_H_
#define _P_DEVICE_H_

#include "system.h"

struct device;

struct device *device_open(const char *pathname);

void device_close(struct device *device);

int device_read(struct device *device, void *buf, uint64_t off, uint64_t len);

int device_write(struct device *device,
		 const void *buf,
		 uint64_t off,
		 uint64_t len);

uint64_t device_size(const struct device *device);

uint64_t device_block(const struct device *device);

#endif /* _DEVICE_H_ */
