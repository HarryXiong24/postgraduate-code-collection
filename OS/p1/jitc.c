/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

/* research the above Needed API and design accordingly */

struct jitc
{
  void *handle;
};

/**
 * Compiles a C program into a dynamically loadable module.
 *
 * input : the file pathname of the C program
 * output: the file pathname of the dynamically loadable module
 *
 * return: 0 on success, otherwise error
 */

int jitc_compile(const char *input, const char *output)
{
  pid_t pid = fork();
  char *args[7];
  args[0] = "gcc";
  args[1] = "-shared";
  args[2] = "-fPIC";
  args[3] = "-o";
  args[4] = (char *)output;
  args[5] = (char *)input;
  args[6] = NULL;

  if (pid == 0)
  {
    /* This block will be executed by child process */
    /* execv(): apply gcc to compile the input C file */
    /* execlp("gcc", "gcc", "-shared", "-fPIC", "-o", output, input, NULL); */
    execv("/usr/bin/gcc", args);
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    return -1; /*  Fork failed */
  }
  else
  {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
      return 0; /* Compilation successful */
    }
    else
    {
      return -1; /* Compilation failed */
    }
  }
}

/**
 * Loads a dynamically loadable module into the calling process' memory for
 * execution.
 *
 * pathname: the file pathname of the dynamically loadable module
 *
 * return: an opaque handle or NULL on error
 */

struct jitc *jitc_open(const char *pathname)
{
  void *handle = dlopen(pathname, RTLD_NOW);
  struct jitc *j = malloc(sizeof(struct jitc));

  if (!handle)
  {
    return NULL;
  }

  if (!j)
  {
    dlclose(handle);
    return NULL;
  }

  j->handle = handle;
  return j;
}

/**
 * Unloads a previously loaded dynamically loadable module.
 *
 * jitc: an opaque handle previously obtained by calling jitc_open()
 *
 * Note: jitc may be NULL
 */

void jitc_close(struct jitc *jitc)
{
  if (jitc)
  {
    if (jitc->handle)
    {
      dlclose(jitc->handle);
    }
    free(jitc);
  }
}

/**
 * Searches for a symbol in the dynamically loaded module associated with jitc.
 *
 * jitc: an opaque handle previously obtained by calling jitc_open()
 *
 * return: the memory address of the start of the symbol, or 0 on error
 */

long jitc_lookup(struct jitc *jitc, const char *symbol)
{
  void *sym_address = dlsym(jitc->handle, symbol);

  if (!jitc || !jitc->handle)
  {
    return 0;
  }

  return (long)sym_address;
}