/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scm.h
 */

#ifndef _SCM_H_
#define _SCM_H_

#include "system.h"

struct scm;

/**
 * Initializes an SCM region using the file specified in pathname as the
 * backing device, opening the regsion for memory allocation activities.
 *
 * pathname: the file pathname of the backing device
 * truncate: if non-zero, truncates the SCM region, clearning all data
 *
 * return: an opaque handle or NULL on error
 */

struct scm *scm_open(const char *pathname, int truncate);

/**
 * Closes a previously opened SCM handle.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * Note: scm may be NULL
 */

void scm_close(struct scm *scm);

/**
 * Analogous to the standard C malloc function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * n  : the size of the requested memory in bytes
 *
 * return: a pointer to the start of the allocated memory or NULL on error
 */

void *scm_malloc(struct scm *scm, size_t n);

/**
 * Analogous to the standard C strdup function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * s  : a C string to be duplicated.
 *
 * return: the base memory address of the duplicated C string or NULL on error
 */

char *scm_strdup(struct scm *scm, const char *s);

/**
 * Analogous to the standard C free function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * p  : a pointer to the start of a previously allocated memory
 */

void scm_free(struct scm *scm, void *p);

/**
 * Returns the number of SCM bytes utilized thus far.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * return: the number of bytes utilized thus far
 */

size_t scm_utilized(const struct scm *scm);

/**
 * Returns the number of SCM bytes available in total.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * return: the number of bytes available in total
 */

size_t scm_capacity(const struct scm *scm);

/**
 * Returns the base memory address withn the SCM region, i.e., the memory
 * pointer that would have been returned by the first call to scm_malloc()
 * after a truncated initialization.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * return: the base memory address within the SCM region
 */

void *scm_mbase(struct scm *scm);

#endif /* _SCM_H_ */
