/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * system.c
 */

#define _GNU_SOURCE

#include <unistd.h>
#include "system.h"

/**
 * Needs:
 *   unlink()
 *   vsnprintf()
 */

void
file_delete(const char *pathname)
{
	if (safe_strlen(pathname)) {
		if (unlink(pathname)) {
			/* ignore */
		}
	}
}

void
safe_sprintf(char *buf, size_t len, const char *format, ...)
{
	va_list ap;

	assert( (!len || buf) && format );

	va_start(ap, format);
	if ((int)len <= vsnprintf(buf, len, format, ap)) {
		va_end(ap);
		EXIT("software");
	}
	va_end(ap);
}

size_t
safe_strlen(const char *s)
{
	return s ? strlen(s) : 0;
}
