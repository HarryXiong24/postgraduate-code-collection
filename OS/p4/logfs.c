/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * logfs.c
 */

#include <pthread.h>
#include "device.h"
#include "logfs.h"

#define WCACHE_BLOCKS 32
#define RCACHE_BLOCKS 256

/**
 * Needs:
 *   pthread_create()
 *   pthread_join()
 *   pthread_mutex_init()
 *   pthread_mutex_destroy()
 *   pthread_mutex_lock()
 *   pthread_mutex_unlock()
 *   pthread_cond_init()
 *   pthread_cond_destroy()
 *   pthread_cond_wait()
 *   pthread_cond_signal()
 */

/* research the above Needed API and design accordingly */
