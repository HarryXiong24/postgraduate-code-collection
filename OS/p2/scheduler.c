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
  Thread *current = state.head;
  Thread *prev = NULL;

  while (current)
  {
    if (current->status == STATUS_TERMINATED)
    {
      if (prev)
      {
        prev->next = current->next;
      }
      else
      {
        state.head = current->next;
      }
      free(current->stack.memory_start);
      free(current);
      if (prev)
      {
        current = prev->next;
      }
      else
      {
        current = state.head;
      }
    }
    else
    {
      prev = current;
      current = current->next;
    }
  }
}

Thread *thread_candidate(void)
{
  Thread *candidate = NULL;

  if (!state.thread)
  {
    return NULL;
  }

  candidate = state.head;

  while (candidate && candidate->status != STATUS_)
  {
    candidate = candidate->next;
  }

  return candidate;
}

void schedule(void)
{
  /* call thread_candidate() to pick up a thread, if not exist return */
  /* state.thread = thread */
  /* thread.status = running */
  /* longjmp(thread -> ctx) */

  Thread *candidate = thread_candidate();

  if (!candidate)
  {
    longjmp(state.ctx, 1); /* Return to scheduler_execute if no candidate */
  }

  state.thread = candidate;

  if (candidate->status == STATUS_)
  {
    if (setjmp(candidate->ctx) == 0)
    {
      uint64_t rsp = (uint64_t)memory_align(candidate->stack.memory_end, page_size());
      __asm__ volatile("mov %[rs], %%rsp \n"
                       : [rs] "+r"(rsp)::);
      candidate->status = STATUS_RUNNING;
      longjmp(candidate->ctx, 1);
    }
    else
    {
      state.thread->fnc(state.thread->arg);
      state.thread->status = STATUS_TERMINATED;
      scheduler_yield();
    }
  }
  else
  {
    longjmp(candidate->ctx, 1);
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

  Thread *new_thread;
  void *stack_memory;
  size_t p_size;

  p_size = page_size();
  if (p_size == 0)
  {
    return -1;
  }

  new_thread = (Thread *)malloc(sizeof(Thread));
  if (!new_thread)
  {
    return -1;
  }

  new_thread->status = STATUS_;
  new_thread->fnc = fnc;
  new_thread->arg = arg;

  stack_memory = malloc(STACK_SIZE + p_size);
  if (!stack_memory)
  {
    free(new_thread);
    return -1;
  }
  new_thread->stack.memory_start = memory_align(stack_memory, p_size);
  new_thread->stack.memory_end = (char *)new_thread->stack.memory_start + STACK_SIZE;

  new_thread->next = state.head;
  state.head = new_thread;

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

  if (setjmp(state.ctx) == 0)
  {
    schedule();
  }
  else
  {
    if (thread_candidate() != NULL) /* Check if there are more threads to run */
    {
      schedule();
    }
  }

  destroy();
}

/**
 * Called from within a user thread to yield the CPU to another user thread.
 */
void scheduler_yield(void)
{
  /* check point about the state thread if it is ctx */
  /* if state.thread -> status = sleeping */
  /* longjmp(state.ctx) */
  if (setjmp(state.thread->ctx) == 0)
  {
    if (state.thread->status == STATUS_SLEEPING)
    {
      longjmp(state.ctx, 1);
    }
  }
}
