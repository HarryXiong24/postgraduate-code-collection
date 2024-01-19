/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * index.h
 */

#ifndef _INDEX_H_
#define _INDEX_H_

#include "system.h"

struct index;

struct index *index_open(void);

void index_close(struct index *index);

uint64_t *index_update(struct index *index, const void *key, uint64_t key_len);

uint64_t *index_lookup(struct index *index, const char *key, uint64_t key_len);

#endif /* _INDEX_H_ */
