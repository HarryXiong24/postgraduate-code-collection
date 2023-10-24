/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scheduler.c
 */

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
  char *stack_backup;       /* Backup for saved stack content */
  size_t stack_backup_size; /* Size of the saved stack content */
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
    free(current->stack.memory);
    free(current->stack.memory_);
    free(current->stack_backup);
    free(current);
    current = next;
  }
  state.head = NULL;
  state.current_thread = NULL;
}

void *get_stack_pointer(void)
{
  void *sp;
  __asm__("mov %%rsp, %0"
          : "=r"(sp));
  return sp;
}

void set_stack_pointer(void *rsp)
{
  __asm__ volatile("mov %[rs], %%rsp \n"
                   : [rs] "+r"(rsp)::);
}

thread *thread_candidate(void)
{
  thread *candidate = state.current_thread ? state.current_thread : state.head;
  thread *start = candidate; /* Save the starting point */

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
  /* call thread_candidate() to pick up a thread, if not exist return */
  /* state.thread = thread */
  /* thread.status = running */
  /* longjmp(thread -> ctx) */

  thread *candidate = thread_candidate();
  printf("Entering schedule()\n");
  if (!candidate)
  {
    return;
  }
  printf("Candidate selected\n");

  if (state.current_thread && state.current_thread->status != STATUS_)
  {
    printf("Saving current thread stack\n");
    /* Save the current stack */
    state.current_thread->stack_backup_size = (char *)state.current_thread->stack.memory - (char *)get_stack_pointer();
    if (state.current_thread->stack_backup_size > 0)
    {
      memcpy(state.current_thread->stack_backup, get_stack_pointer(), state.current_thread->stack_backup_size);
    }
    state.current_thread->status = STATUS_SLEEPING;
  }

  if (candidate->status != STATUS_)
  {
    printf("Restoring candidate thread stack\n");
    /* Restore the next thread's stack */
    set_stack_pointer((char *)candidate->stack.memory - candidate->stack_backup_size);
    memcpy(get_stack_pointer(), candidate->stack_backup, candidate->stack_backup_size);
  }

  printf("Switch to the candidate thread\n");
  state.current_thread = candidate;
  candidate->status = STATUS_RUNNING;
  longjmp(candidate->ctx, 1); /* Switch to the candidate thread */
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

  printf("Entering scheduler_create()\n");

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

  new_thread->stack.memory_ = malloc(2 * p_size);
  if (!new_thread->stack.memory_)
  {
    free(new_thread);
    return -1;
  }
  new_thread->stack.memory = (char *)new_thread->stack.memory_ + 2 * p_size - sizeof(void *); /* Allocating memory for stack_backup */
  new_thread->stack_backup = malloc(2 * p_size);                                              /* Allocate memory for stack_backup */
  if (!new_thread->stack_backup)
  {
    free(new_thread->stack.memory_);
    free(new_thread->stack_backup);
    free(new_thread);
    return -1;
  }

  printf("Malloc End\n");

  if (setjmp(new_thread->ctx) != 0)
  {
    new_thread->fnc(new_thread->arg);
    new_thread->status = STATUS_TERMINATED;
    longjmp(state.ctx, 1);
  }

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

  destroy();
}

/**
 * Called from within a user thread to yield the CPU to another user thread.
 */
void scheduler_yield(void)
{
  printf("Entering scheduler_yield()\n");
  /* check point about the state thread if it is ctx */
  /* state.thread -> status = sleeping */
  /* longjmp(state.ctx) */
  if (setjmp(state.current_thread->ctx) == 0)
  {

    state.current_thread->status = STATUS_SLEEPING;
    longjmp(state.ctx, 1); /* Jump back to scheduler_execute */
  }
  state.current_thread->status = STATUS_RUNNING;
  /* If we're here, it means this thread has been rescheduled after yielding */
}
