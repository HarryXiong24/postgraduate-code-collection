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
    void *memory_;
    void *memory;
  } stack;
  jmp_buf ctx;
  scheduler_fnc_t fnc;
  char *arg;
  struct Thread *next;
} thread;

static struct
{
  thread *head;
  thread *current_thread;
  jmp_buf ctx;
} state;

void destroy(void)
{
  thread *current = state.head;
  while (current != NULL)
  {
    thread *next = current->next;
    if (current)
    {
      free(current);
    }
    if (current->stack.memory)
    {
      free(current->stack.memory);
    }
    if (current->stack.memory_)
    {
      free(current->stack.memory_);
    }
    current = next;
  }
  state.head = NULL;
  state.current_thread = NULL;
}

thread *thread_candidate(void)
{
  thread *candidate;
  thread *start; /* Save the starting point */

  if (!state.current_thread)
  {
    return state.head; /* If no current thread, return the head. */
  }

  candidate = state.current_thread->next ? state.current_thread->next : state.head;
  start = candidate;

  do
  {
    if (candidate->status == STATUS_ || candidate->status == STATUS_SLEEPING)
    {
      return candidate;
    }
    candidate = candidate->next ? candidate->next : state.head;
  } while (candidate != start);

  return NULL;
}

void schedule(void)
{
  thread *candidate = thread_candidate();

  if (!candidate)
  {
    return;
  }

  state.current_thread = candidate;

  if (candidate->status == STATUS_)
  {

    uint64_t *rsp = (uint64_t *)candidate->stack.memory;
    __asm__ volatile("mov %[rs], %%rsp \n"
                     : [rs] "+r"(rsp)::);

    candidate->status = STATUS_RUNNING;
    candidate->fnc(candidate->arg);
    candidate->status = STATUS_TERMINATED;
    longjmp(state.ctx, 1);
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
  thread *new_thread;
  size_t p_size;

  p_size = page_size();
  if (p_size == 0)
  {
    return -1;
  }

  new_thread = (thread *)malloc(sizeof(thread));
  if (!new_thread)
  {
    return -1;
  }

  new_thread->status = STATUS_;
  new_thread->fnc = fnc;
  new_thread->arg = arg;
  new_thread->next = NULL;

  new_thread->stack.memory_ = malloc(2 * p_size);
  if (!new_thread->stack.memory_)
  {
    return -1;
  }
  new_thread->stack.memory = memory_align(new_thread->stack.memory_, p_size); /* Allocate memory for stack_backup */

  /* link together */
  /* Here needs to make sure that new thread should be linked at last so that we can have hello->world->I->love->this->course */
  if (state.head == NULL)
  {
    /* If list is empty, set the head to the new thread */
    state.head = new_thread;
  }
  else
  {
    thread *current = state.head;
    while (current->next != NULL)
    {
      current = current->next;
    }
    current->next = new_thread;
  }

  return 0;
}

/* Signal handler for SIGALRM */
/*
 * Use the alarm function to set a timer that sends a SIGALRM signal after a specified number of seconds.
 * Define a signal processing function that will be called when the SIGALRM signal is received
 * scheduler_yield() will be automatically called after 1s to trigger thread switching
 */
/*
  void alarm_handler(int signum)
  {
    UNUSED(signum);
    scheduler_yield();
  }
*/

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
  /* Set the signal handler #define SIGALRM 14 - alarm clock */
  /* signal(SIGALRM, alarm_handler); */

  /* Set an alarm for every 1 second (or any desired interval) */
  /* alarm(1); */

  if (setjmp(state.ctx) == 0)
  {
    schedule();
  }
  else
  {
    schedule();
    return;
  }

  destroy();
}

/**
 * Called from within a user thread to yield the CPU to another user thread.
 */
void scheduler_yield(void)
{
  /* check point about the state thread if it is ctx */
  /* state.thread -> status = sleeping */
  /* longjmp(state.ctx) */
  if (setjmp(state.current_thread->ctx) == 0)
  {

    state.current_thread->status = STATUS_SLEEPING;
    longjmp(state.ctx, 1); /* Jump back to scheduler_execute */
  }
}
