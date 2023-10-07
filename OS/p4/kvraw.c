/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * kvraw.c
 */

#include "logfs.h"
#include "kvraw.h"

#define META_LEN ( sizeof (struct meta) )

#define KEY_OFF(o) ( (o) + META_LEN )
#define VAL_OFF(o) ( (o) + META_LEN + meta.key_len )

struct kvraw {
	uint64_t size;
	struct logfs *logfs;
};

#pragma pack(push, 1)
struct meta {
	char mark[2];
	uint64_t off;
	uint16_t key_len;
	uint32_t val_len;
};
#pragma pack(pop)

static int
read_meta(struct kvraw *kvraw, uint64_t off, struct meta *meta)
{
	memset(meta, 0, sizeof (struct meta));
	if ((off + META_LEN) > kvraw->size) {
		TRACE("corrupt data");
		return -1;
	}
	if (logfs_read(kvraw->logfs, meta, off, META_LEN)) {
		TRACE(0);
		return -1;
	}
	if (('K' != meta->mark[0]) ||
	    ('V' != meta->mark[1]) ||
	    ((off + META_LEN + meta->key_len + meta->val_len) > kvraw->size)) {
		TRACE("corrupt data");
		return -1;
	}
	return 0;
}

struct kvraw *
kvraw_open(const char *pathname)
{
	struct kvraw *kvraw;
	uint64_t off;

	assert( safe_strlen(pathname) );

	if (!(kvraw = malloc(sizeof (struct kvraw)))) {
		TRACE("out of memory");
		return NULL;
	}
	memset(kvraw, 0, sizeof (struct kvraw));
	if (!(kvraw->logfs = logfs_open(pathname))) {
		kvraw_close(kvraw);
		TRACE(0);
		return NULL;
	}
	if (kvraw_append(kvraw, "", 1, "", 1, &off)) { /* off = 0 */
		kvraw_close(kvraw);
		TRACE(0);
		return NULL;
	}
	return kvraw;
}

void
kvraw_close(struct kvraw *kvraw)
{
	if (kvraw) {
		logfs_close(kvraw->logfs);
		memset(kvraw, 0, sizeof (struct kvraw));
	}
	FREE(kvraw);
}

int
kvraw_lookup(struct kvraw *kvraw,
	     void *key,
	     uint64_t *key_len, /* in/out */
	     void *val,
	     uint64_t *val_len, /* in/out */
	     uint64_t *off)     /* in/out */
{
	uint64_t key_len_, val_len_;
	struct meta meta;

	assert( kvraw );
	assert( key_len && (!(*key_len) || key) );
	assert( val_len && (!(*val_len) || val) );
	assert( off && (*off) );

	if (read_meta(kvraw, (*off), &meta)) {
		TRACE(0);
		return -1;
	}
	key_len_ = MIN(meta.key_len, (*key_len));
	val_len_ = MIN(meta.val_len, (*val_len));
	if (logfs_read(kvraw->logfs, key, KEY_OFF(*off), key_len_) ||
	    logfs_read(kvraw->logfs, val, VAL_OFF(*off), val_len_)) {
		TRACE(0);
		return -1;
	}
	(*key_len) = meta.key_len;
	(*val_len) = meta.val_len;
	(*off) = meta.off;
	return 0;
}

int
kvraw_append(struct kvraw *kvraw,
	     const void *key,
	     uint64_t key_len,
	     const void *val,
	     uint64_t val_len,
	     uint64_t *off)
{
	struct meta meta;
	uint64_t off_;

	assert( kvraw );
	assert( key && key_len && (0xffff >= key_len) );
	assert( (!val_len || val) && (0xffffffff >= val_len) );
	assert( off );

	off_ = kvraw->size;
	meta.mark[0] = 'K';
	meta.mark[1] = 'V';
	meta.off = (*off);
	meta.key_len = (uint16_t)key_len;
	meta.val_len = (uint32_t)val_len;
	if (logfs_append(kvraw->logfs, &meta, META_LEN) ||
	    logfs_append(kvraw->logfs, key, meta.key_len) ||
	    logfs_append(kvraw->logfs, val, meta.val_len)) {
		TRACE(0);
		return -1;
	}
	kvraw->size += META_LEN + meta.key_len + meta.val_len;
	(*off) = off_;
	return 0;
}
