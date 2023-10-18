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
  jmp_buf ctx;
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
  scheduler_fnc_t fnc;
  char *arg;
  struct Thread *next;
} Thread;

static struct
{
  Thread *head;
  Thread *thread;
  jmp_buf ctx;
} state;

int scheduler_create(scheduler_fnc_t fnc, void *arg)
{
  Thread *new_thread = (Thread *)malloc(sizeof(Thread));
  if (!new_thread)
  {
    return -1; /* Memory allocation failed */
  }

  /* Allocate memory for the stack */
  new_thread->stack.memory_start = malloc(STACK_SIZE);
  if (!new_thread->stack.memory_start)
  {
    free(new_thread);
    return -1; /* Memory allocation for stack failed */
  }
  new_thread->stack.memory_end = (char *)new_thread->stack.memory_start + STACK_SIZE;

  /* Set the thread's function and initial status */
  new_thread->fnc = fnc;
  new_thread->arg = arg;
  new_thread->status = STATUS_;

  /* Link the new thread to the list */
  if (!state.head)
  {
    state.head = (Thread *)new_thread;
  }
  else
  {
    Thread *temp = state.head;
    while (temp->next)
    {
      temp = temp->next;
    }
    temp->next = new_thread;
  }
  new_thread->next = NULL;

  return 0; /* Successful creation */
}

static inline void thread_start(Thread *thread)
{
  thread->fnc(thread->arg);           /* Call the thread function */
  thread->status = STATUS_TERMINATED; /* Mark thread as terminated */
  scheduler_yield();                  /* Yield to the next thread */
}

void scheduler_execute(void)
{
  if (setjmp(state.ctx) == 0)
  {
    /* Save the scheduler's context and jump to the first thread */
    state.thread = state.head;
    if (state.thread)
    {
      /* Set up the thread's stack and call thread_start */
      char *stack_top = (char *)memory_align(state.thread->stack.memory_end, 16);
      __asm__ volatile(
          "mov sp, %0\n"
          "mov x0, %1\n"
          "bl thread_start"
          :
          : "r"(stack_top), "r"(state.thread)
          : "memory");
    }
  }

  /* Cleanup terminated threads */
  while (state.head && state.head->status == STATUS_TERMINATED)
  {
    Thread *temp = state.head;
    state.head = state.head->next;
    free(temp->stack.memory_start);
    free(temp);
  }
}

void scheduler_yield(void)
{
  if (setjmp(state.thread->ctx) == 0)
  {
    /* Save the current thread's context */
    state.thread = state.thread->next;
    if (!state.thread)
    {
      /* If there's no next thread, jump back to the scheduler */
      longjmp(state.ctx, 1);
    }
    else
    {
      /* Otherwise, jump to the next thread */
      longjmp(state.thread->ctx, 1);
    }
  }
}
