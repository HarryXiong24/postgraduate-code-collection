/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.h
 */

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/**
 * scheduler_fnc_t defines the signature of the user thread function to
 * be scheduled by the scheduler. The user thread function will be supplied
 * with a copy of the arg specified when creating the thread.
 */

typedef void (*scheduler_fnc_t)(void *arg);

/**
 * Creates a new user thread.
 *
 * fnc: the start function of the user thread (see scheduler_fnc_t)
 * arg: a pass-through pointer defining the context of the user thread
 *
 * return: 0 on success, otherwise error
 */

int scheduler_create(scheduler_fnc_t fnc, void *arg);

/**
 * Called to execute the user threads previously created by calling
 * scheduler_create().
 *
 * Notes:
 *   * This function should be called after a sequence of 0 or more
 *     scheduler_create() calls.
 *   * This function returns after all user threads (previously created)
 *     have terminated.
 *   * This function is not re-enterant.
 */

void scheduler_execute(void);

/**
 * Called from within a user thread to yield the CPU to another user thread.
 */

void scheduler_yield(void);

#endif /* _SCHEDULER_H_ */
