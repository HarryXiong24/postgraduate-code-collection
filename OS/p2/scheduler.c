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

#define MAX_THREADS 32

typedef struct Thread
{
  scheduler_fnc_t fnc;
  void *arg;
  jmp_buf context;
  struct Thread *next;
} Thread;

static Thread *current_thread = NULL;
static Thread *threads = NULL;
static jmp_buf main_context;
static int num_threads = 0;

int scheduler_create(scheduler_fnc_t fnc, void *arg)
{
  Thread *thread = NULL;
  if (num_threads >= MAX_THREADS)
  {
    return -1;
  }

  thread = (Thread *)malloc(sizeof(Thread));
  if (!thread)
  {
    return -1;
  }

  thread->fnc = fnc;
  thread->arg = arg;
  thread->next = NULL;

  if (!threads)
  {
    threads = thread;
  }
  else
  {
    Thread *tmp = threads;
    while (tmp->next)
    {
      tmp = tmp->next;
    }
    tmp->next = thread;
  }

  num_threads++;
  return 0;
}

void scheduler_execute(void)
{
  if (!threads)
  {
    return;
  }

  if (setjmp(main_context) == 0)
  {
    current_thread = threads;
    longjmp(current_thread->context, 1);
  }
  else
  {
    while (threads)
    {
      Thread *tmp = threads;
      threads = threads->next;
      free(tmp);
    }
  }
}

void scheduler_yield(void)
{
  while (num_threads > 0)
  {
    if (setjmp(current_thread->context) == 0)
    {
      current_thread = current_thread->next ? current_thread->next : threads;
      longjmp(current_thread->context, 1);
    }
    else
    {
      Thread *tmp = NULL;
      current_thread->fnc(current_thread->arg);
      tmp = current_thread;
      if (current_thread->next)
      {
        current_thread = current_thread->next;
      }
      else
      {
        current_thread = threads;
      }
      free(tmp);
      num_threads--;
    }
  }

  if (num_threads == 0)
  {
    longjmp(main_context, 1);
  }
}
