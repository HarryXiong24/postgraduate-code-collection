/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * index.c
 */

#include "index.h"

#define LOAD 0.70

struct index {
	uint64_t size;
	uint64_t capacity;
	struct {
		uint64_t key;
		uint64_t off;
	} *maps;
};

static uint64_t
hash(const void *buf, uint64_t len)
{
	uint64_t i, a, b, c, d;
	const char *p;

	assert( !len || buf );

	p = (const char *)buf;
	a = b = c = d = len * 414507281407;
	for (i=0; i<len; ++i) {
		a += (uint64_t)p[i] * 592821132889;
		b += (uint64_t)p[i] * 963726515729;
		c += (uint64_t)p[i] * 1765037224331;
		d += (uint64_t)p[i] * 2428095424619;
		d ^= c; c = (c << 15) | (c >> (64 - 15)); d += c;
		a ^= d; d = (d << 52) | (d >> (64 - 52)); a += d;
		b ^= a; a = (a << 26) | (a >> (64 - 26)); b += a;
		c ^= b; b = (b << 51) | (b >> (64 - 51)); c += b;
		d ^= c; c = (c << 28) | (c >> (64 - 28)); d += c;
		a ^= d; d = (d <<  9) | (d >> (64 -  9)); a += d;
		b ^= a; a = (a << 47) | (a >> (64 - 47)); b += a;
		c ^= b; b = (b << 54) | (b >> (64 - 54)); c += b;
		d ^= c; c = (c << 32) | (c >> (64 - 32)); d += c;
		a ^= d; d = (d << 25) | (d >> (64 - 25)); a += d;
		b ^= a; a = (a << 63) | (a >> (64 - 63)); b += a;
	}
	return a + b + c + d;
}

static void
destroy(struct index *index)
{
	FREE(index->maps);
	memset(index, 0, sizeof (struct index));
}

static int
create(struct index *index, uint64_t capacity)
{
	uint64_t n;

	memset(index, 0, sizeof (struct index));
	index->capacity = capacity;
	n = index->capacity * sizeof (index->maps[0]);
	if (!(index->maps = malloc(n))) {
		destroy(index);
		TRACE("out of memory");
		return -1;
	}
	memset(index->maps, 0, n);
	return 0;
}

static uint64_t *
update(struct index *index, uint64_t key)
{
	uint64_t i, j;

	for (i=0; i<index->capacity; ++i) {
		j = (key + i) % index->capacity;
		if (!index->maps[j].key) { /* insert */
			index->maps[j].key = key;
			index->maps[j].off = 0;
			++index->size;
			return &index->maps[j].off;
		}
		if (index->maps[j].key == key) { /* update */
			index->maps[j].key = key;
			return &index->maps[j].off;
		}
	}
	EXIT("software");
	return NULL;
}

static int
grow(struct index *index)
{
	struct index index_;
	double load;
	uint64_t i;

	load = index->capacity ? ((double)index->size / index->capacity) : 1.0;
	if (LOAD < load) {
		if (create(&index_, (index->capacity + 97) * 3 / 2)) {
			TRACE(0);
			return -1;
		}
		for (i=0; i<index->capacity; ++i) {
			if (index->maps[i].key) {
				*(update(&index_, index->maps[i].key)) =
					index->maps[i].off;
			}
		}
		destroy(index);
		(*index) = index_;
	}
	return 0;
}

struct index *
index_open(void)
{
	struct index *index;

	if (!(index = malloc(sizeof (struct index)))) {
		TRACE("out of memory");
		return NULL;
	}
	memset(index, 0, sizeof (struct index));
	return index;
}

void
index_close(struct index *index)
{
	if (index) {
		FREE(index->maps);
		memset(index, 0, sizeof (struct index));
	}
	FREE(index);
}

uint64_t *
index_update(struct index *index, const void *key_, uint64_t key_len)
{
	uint64_t key;

	assert( key_ && key_len );

	if (grow(index)) {
		TRACE(0);
		return NULL;
	}
	key = hash(key_, key_len);
	key = key ? key : (key + 1);
	return update(index, key);
}

uint64_t *
index_lookup(struct index *index, const char *key_, uint64_t key_len)
{
	uint64_t i, j, key;

	assert( key_ && key_len );

	key = hash(key_, key_len);
	key = key ? key : (key + 1);
	for (i=0; i<index->capacity; ++i) {
		j = (key + i) % index->capacity;
		if (!index->maps[j].key) {
			break;
		}
		if (index->maps[j].key == key) {
			return &index->maps[j].off;
		}
	}
	return NULL;
}
