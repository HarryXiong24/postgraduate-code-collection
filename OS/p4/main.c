/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include "term.h"
#include "kvdb.h"

#define SLEN(s) ( safe_strlen(s) + 1 )

#define TEST(f,m)						\
	do {							\
		uint64_t t = ref_time();			\
		if (f()) {					\
			t = ref_time() - t;			\
			term_color(TERM_COLOR_RED);		\
			term_bold();				\
			printf("\t [FAIL] ");			\
			term_reset();				\
			printf("%20s %6.1fs\n", (m), 1e-6*t);	\
		}						\
		else {						\
			t = ref_time() - t;			\
			term_color(TERM_COLOR_GREEN);		\
			term_bold();				\
			printf("\t [PASS] ");			\
			term_reset();				\
			printf("%20s %6.1fs\n", (m), 1e-6*t);	\
		}						\
	} while (0)

static const char *PATHNAME;

static void
mk_object(char *key,
	  char *val,
	  uint64_t key_len,
	  uint64_t val_len,
	  uint64_t *key_len_,
	  uint64_t *val_len_,
	  uint64_t i,
	  char code)
{
	const uint64_t MIN_LEN = 16;
	char key_[16], val_[16];
	uint64_t j, h;

	assert( MIN_LEN < key_len );
	assert( MIN_LEN < val_len );

	srand((int)i + 100);
	h = ((uint64_t)rand() << 32) + (uint64_t)rand();
	(*key_len_) = MIN_LEN + (h % (key_len - MIN_LEN));
	(*val_len_) = MIN_LEN + (h % (val_len - MIN_LEN));
	for (j=0; j<(*key_len_); ++j) {
		key[j] = (char)(h % 256);
	}
	for (j=0; j<(*val_len_); ++j) {
		val[j] = (char)(h % 256);
	}
	safe_sprintf(key_, MIN_LEN, "k%lu", (unsigned long)i);
	safe_sprintf(val_, MIN_LEN, "%c%lu", code, (unsigned long)i);
	memmove(key + (*key_len_) - MIN_LEN, key_, safe_strlen(key_));
	memmove(val + (*val_len_) - MIN_LEN, val_, safe_strlen(val_));
}

static int
read_write(const uint64_t N, const uint64_t K, const uint64_t V)
{
	uint64_t i, j, key_len, val_len, val_len_;
	void *key, *val, *val_;
	struct kvdb *kvdb;

	key = val = val_ = NULL;
	if (!(kvdb = kvdb_open(PATHNAME))) {
		TRACE(0);
		return -1;
	}
	if (!(key = malloc(K)) || !(val = malloc(V)) || !(val_ = malloc(V))) {
		kvdb_close(kvdb);
		FREE(key);
		FREE(val);
		FREE(val_);
		TRACE("out of memory");
		return -1;
	}

	/* insert */

	for (i=0; i<N; ++i) {
		mk_object(key, val, K, V, &key_len, &val_len, i, 'i');
		val_len_ = V;
		if (kvdb_insert(kvdb, key, key_len, val, val_len) ||
		    kvdb_lookup(kvdb, key, key_len, val_, &val_len_) ||
		    (val_len != val_len_) ||
		    memcmp(val, val_, val_len_) ||
		    ((i + 1) != kvdb_size(kvdb)) ||
		    (0 != kvdb_waste(kvdb))) {
			kvdb_close(kvdb);
			FREE(key);
			FREE(val);
			FREE(val_);
			TRACE("software");
			return -1;
		}
	}

	/* lookup */

	for (i=0; i<N; ++i) {
		j = rand() % N;
		mk_object(key, val, K, V, &key_len, &val_len, j, 'i');
		val_len_ = V;
		if (kvdb_lookup(kvdb, key, key_len, val_, &val_len_) ||
		    (val_len != val_len_) ||
		    memcmp(val, val_, val_len_) ||
		    (N != kvdb_size(kvdb)) ||
		    (0 != kvdb_waste(kvdb))) {
			kvdb_close(kvdb);
			FREE(key);
			FREE(val);
			FREE(val_);
			TRACE("software");
			return -1;
		}
	}
	return 0;
}

static int read_write_large(void)  { return read_write(1234, 1234, 1234); }
static int read_write_small(void)  { return read_write(1234,  123,  123); }
static int read_write_single(void) { return read_write(   1,   23,   23); }

static int
heavy_rewrite(void)
{
	const char * const KEY = "KEY";
	uint64_t i, n, val_len_;
	char val[64], val_[64];
	struct kvdb *kvdb;

	n = 9876;
	if (!(kvdb = kvdb_open(PATHNAME))) {
		TRACE(0);
		return -1;
	}
	for (i=0; i<n; ++i) {
		safe_sprintf(val, sizeof (val), "v%lu", (unsigned long)i);
		val_len_ = sizeof (val_);
		if (kvdb_update(kvdb, KEY, SLEN(KEY), val, SLEN(val)) ||
		    kvdb_lookup(kvdb, KEY, SLEN(KEY), val_, &val_len_) ||
		    (SLEN(val) != val_len_) ||
		    memcmp(val, val_, val_len_) ||
		    (1 != kvdb_size(kvdb)) ||
		    (i != kvdb_waste(kvdb))) {
			kvdb_close(kvdb);
			TRACE("software");
			return -1;
		}
	}
	kvdb_close(kvdb);
	return 0;
}

static int
basic_logic(void)
{
	const char * const KEY = "KEY";
	const char * const VAL1 = "VAL";
	const char * const VAL2 = "val2";
	const char * const VAL3 = "val2VAL";
	struct kvdb *kvdb;
	uint64_t val_len;
	char val[32];

	if (!(kvdb = kvdb_open(PATHNAME))) {
		TRACE(0);
		return -1;
	}
	if ((0 != kvdb_size(kvdb)) || (0 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* invalid lookup */

	val_len = 0;
	if ((+1 != kvdb_lookup(kvdb, KEY, SLEN(KEY), 0, 0)) ||
	    (+1 != kvdb_lookup(kvdb, KEY, SLEN(KEY), 0, &val_len)) ||
	    (+1 != kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len)) ||
	    (0 != kvdb_size(kvdb)) ||
	    (0 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* invalid replace */

	if ((+1 != kvdb_replace(kvdb, KEY, SLEN(KEY), VAL1, SLEN(VAL1))) ||
	    (0 != kvdb_size(kvdb)) ||
	    (0 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* insert, lookup, re-insert, lookup */

	val_len = sizeof (val);
	if (kvdb_insert(kvdb, KEY, SLEN(KEY), VAL1, SLEN(VAL1)) ||
	    kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL1) != val_len) ||
	    memcmp(VAL1, val, val_len) ||
	    (+1 != kvdb_insert(kvdb, KEY, SLEN(KEY), VAL1, SLEN(VAL1))) ||
	    kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL1) != val_len) ||
	    memcmp(VAL1, val, val_len) ||
	    (1 != kvdb_size(kvdb)) ||
	    (0 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* replace, lookup, re-insert, lookup */

	val_len = sizeof (val);
	if (kvdb_replace(kvdb, KEY, SLEN(KEY), VAL2, SLEN(VAL2)) ||
	    kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL2) != val_len) ||
	    memcmp(VAL2, val, val_len) ||
	    (+1 != kvdb_insert(kvdb, KEY, SLEN(KEY), VAL2, SLEN(VAL2))) ||
	    kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL2) != val_len) ||
	    memcmp(VAL2, val, val_len) ||
	    (1 != kvdb_size(kvdb)) ||
	    (1 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* update, lookup, re-insert, lookup */

	val_len = sizeof (val);
	if (kvdb_update(kvdb, KEY, SLEN(KEY), VAL3, SLEN(VAL3)) ||
	    kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL3) != val_len) ||
	    memcmp(VAL3, val, val_len) ||
	    (+1 != kvdb_insert(kvdb, KEY, SLEN(KEY), VAL3, SLEN(VAL3))) ||
	    kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL3) != val_len) ||
	    memcmp(VAL3, val, val_len) ||
	    (1 != kvdb_size(kvdb)) ||
	    (2 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* lookup */

	val_len = sizeof (val);
	if (kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL3) != val_len) ||
	    memcmp(VAL3, val, val_len) ||
	    (1 != kvdb_size(kvdb)) ||
	    (2 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}

	/* remove */

	val_len = sizeof (val);
	if (kvdb_remove(kvdb, KEY, SLEN(KEY), val, &val_len) ||
	    (SLEN(VAL3) != val_len) ||
	    memcmp(VAL3, val, val_len) ||
	    (+1 != kvdb_lookup(kvdb, KEY, SLEN(KEY), val, &val_len)) ||
	    (0 != kvdb_size(kvdb)) ||
	    (3 != kvdb_waste(kvdb))) {
		kvdb_close(kvdb);
		TRACE("software");
		return -1;
	}
	kvdb_close(kvdb);
	return 0;
}

int
main(int argc, char *argv[])
{
	if (2 != argc) {
		printf("usage: %s block-device\n", argv[0]);
		return -1;
	}

	/* initialize */

	PATHNAME = argv[1];
	term_init(0);

	/* prelude */

	term_bold();
	term_color(TERM_COLOR_BLUE);
	printf("---------- TEST BEG ----------\n");
	term_reset();

	/* test */

	TEST(basic_logic, "basic_logic");
	TEST(heavy_rewrite, "heavy_rewrite");
	TEST(read_write_single, "read_write_single");
	TEST(read_write_small, "read_write_small");
	TEST(read_write_large, "read_write_large");

	/* postlude */

	term_bold();
	term_color(TERM_COLOR_BLUE);
	printf("---------- TEST END ----------\n");
	term_reset();
	return 0;
}
