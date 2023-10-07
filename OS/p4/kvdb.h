/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * kvdb.h
 */

#ifndef _KVDB_H_
#define _KVDB_H_

#include "system.h"

#define KVDB_MAX_KEY_LEN 0xffff
#define KVDB_MAX_VAL_LEN 0xffffffff

struct kvdb;

struct kvdb *kvdb_open(const char *pathname);

void kvdb_close(struct kvdb *kvdb);

int /* -1|0|+1 */
kvdb_remove(struct kvdb *kvdb,
	    const void *key,
	    uint64_t key_len,
	    void *val,
	    uint64_t *val_len); /* in/out */

int /* -1|0|+1 */
kvdb_insert(struct kvdb *kvdb,
	    const void *key,
	    uint64_t key_len,
	    const void *val,
	    uint64_t val_len);

int /* -1|0|+1 */
kvdb_update(struct kvdb *kvdb,
	    const void *key,
	    uint64_t key_len,
	    const void *val,
	    uint64_t val_len);

int /* -1|0|+1 */
kvdb_replace(struct kvdb *kvdb,
	     const void *key,
	     uint64_t key_len,
	     const void *val,
	     uint64_t val_len);

int /* -1|0|+1 */
kvdb_lookup(struct kvdb *kvdb,
	    const void *key,
	    uint64_t key_len,
	    void *val,
	    uint64_t *val_len); /* in/out */

uint64_t kvdb_size(const struct kvdb *kvdb);

uint64_t kvdb_waste(const struct kvdb *kvdb);

#endif /* _KVDB_H_ */
