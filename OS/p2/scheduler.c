/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.c
 */

#undef _FORTIFY_SOURCE

#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include "system.h"
#include "scheduler.h"

/**
 * Needs:
 *   setjmp()
 *   longjmp()
 */

/* research the above Needed API and design accordingly */

/* Define a structure for thread */
#define STACK_SIZE 4096

typedef struct Thread
{
  enum
  {
    STATUS_,
    STATUS_RUNNING,
    STATUS_SLEEPING,
    STATUS_TERMINATED
  } status;
  struct
  {
    void *memory_start;
    void *memory_end;
  } stack;
  jmp_buf ctx;
  struct Thread *next;
  scheduler_fnc_t fnc;
  char *arg;
} Thread;

static struct
{
  Thread *head;
  Thread *thread;
  jmp_buf ctx;
} state;

void destroy(void)
{
  Thread *thread = state.head;
  Thread *prev = NULL;

  while (thread)
  {
    if (thread->status == STATUS_TERMINATED)
    {
      if (prev)
      {
        prev->next = thread->next;
        free(thread->stack.memory_start);
        free(thread);
        thread = prev->next;
      }
      else
      {
        state.head = thread->next;
        free(thread->stack.memory_start);
        free(thread);
        thread = state.head;
      }
    }
    else
    {
      prev = thread;
      thread = thread->next;
    }
  }
}

Thread *thread_candidate(void)
{
  Thread *candidate;

  if (!state.thread)
    return NULL;

  candidate = state.thread->next;
  while (candidate->status == STATUS_TERMINATED)
  {
    state.thread->next = candidate->next;
    destroy();
    candidate = state.thread->next;
    if (candidate == state.thread)
    {
      if (state.thread->status == STATUS_TERMINATED)
      {
        destroy();
        return NULL;
      }
      break;
    }
  }
  return candidate;
}

void schedule(void)
{
  /* call thread_candidate() to pick up a thread, if not exist return */

  /* state.thread = thread */

  /* thread.status = running */

  /* longjmp(thread -> ctx) */
  if (setjmp(state.thread->ctx) == 0)
  {
    state.thread = thread_candidate();
    if (state.thread)
      longjmp(state.thread->ctx, 1);
    else
      longjmp(state.ctx, 1);
  }
}

/**
 * Creates a new user thread.
 *
 * fnc: the start function of the user thread (see scheduler_fnc_t)
 * arg: a pass-through pointer defining the context of the user thread
 *
 * return: 0 on success, otherwise error
 */

int scheduler_create(scheduler_fnc_t fnc, void *arg)
{
  /* figure out what's the page size */

  /* allocate thread struct and initialize */

  /* Allocate memory for the stack */

  /* link the new thread to state head */

  /* return */

  uint64_t rsp;
  Thread *thread = (Thread *)malloc(sizeof(Thread));

  if (!thread)
  {
    EXIT("malloc()");
    return -1;
  }

  thread->stack.memory_start = malloc(8 * page_size());
  if (!thread->stack.memory_start)
  {
    free(thread);
    EXIT("malloc()");
    return -1;
  }
  thread->stack.memory_end = (char *)memory_align((char *)thread->stack.memory_start + 8 * page_size(), page_size());

  thread->fnc = fnc;
  thread->arg = arg;
  thread->status = STATUS_RUNNING;

  if (setjmp(thread->ctx) != 0)
  {
    thread->fnc(thread->arg);
    thread->status = STATUS_TERMINATED;
    schedule();
  }

  rsp = (uint64_t)memory_align(thread->stack.memory_end, page_size());
  __asm__ volatile("mov %[rs], %%rsp \n"
                   : [rs] "+r"(rsp)::);

  if (!state.head)
  {
    state.head = thread;
    thread->next = thread;
  }
  else
  {
    thread->next = state.head->next;
    state.head->next = thread;
    state.head = thread;
  }

  return 0;
}

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

void scheduler_execute(void)
{
  /* check point -> state ctx */

  /* schedule(), let a thread run */

  /* destroy() */

  if (!state.head)
  {
    return;
  }

  state.thread = state.head;
  if (setjmp(state.ctx) == 0)
  {
    longjmp(state.thread->ctx, 1);
  }
}

/**
 * Called from within a user thread to yield the CPU to another user thread.
 */
void scheduler_yield(void)
{
  /* check point about the state thread if it is ctx */

  /* if state.thread -> status = sleeping */

  /* longjmp(state.ctx) */
  schedule();
}
