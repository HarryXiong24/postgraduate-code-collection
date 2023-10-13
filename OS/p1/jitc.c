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

/* Define structure jitc */
struct jitc
{
  void *handle;
};

/**
 * Compiles a C program into a dynamically loadable module.
 *
 * C -> .so file
 *
 * input : the file pathname of the C program
 * output: the file pathname of the dynamically loadable module
 *
 * return: 0 on success, otherwise error
 */
int jitc_compile(const char *input, const char *output)
{
  char *args[13];
  pid_t pid = fork();
  /*
    Create a new child process, When execv() functions are called, it replaces the current process's image with the specified new program. This means that once execv is called, the currently running program no longer continues execution but is replaced by GCC. In order for the original program to continue running and wait for GCC to complete compilation, execv needs to be called in the child process.
  */
  if (pid == 0)
  {
    /* This block will be executed by child process */
    /* execv(): apply gcc to compile the input C file */
    args[0] = "gcc";
    args[1] = "-shared";
    args[2] = "-fPIC";
    args[3] = "-o";
    args[4] = (char *)output;
    args[5] = (char *)input;
    args[6] = "main.o"; /* Link with the main object file */
    args[7] = "parser.o";
    args[8] = "system.o";
    args[9] = "lexer.o";
    args[10] = "jitc.o";
    args[11] = "-lm"; /*  Link with math library */
    args[12] = NULL;
    /* Compile input file to a dynamically loadable module */
    execv("/usr/bin/gcc", args);
    exit(EXIT_SUCCESS);
  }
  else if (pid < 0)
  {
    return -1; /* Fork failed */
  }
  else
  {
    int status;
    /* Use waitpid() to wait for the child process (GCC compiler) to complete. */
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
      return 0; /* Compile successful */
    }
    else
    {
      return -1; /* Compile failed */
    }
  }
}

/**
 * Loads a dynamically loadable module into the calling process' memory for
 * execution.
 *
 * open .so file
 *
 * pathname: the file pathname of the dynamically loadable module
 *
 * return: an opaque handle or NULL on error
 */
struct jitc *jitc_open(const char *pathname)
{
  char relative_path[100] = "./";
  void *handle;
  struct jitc *j = NULL;

  /*
    dlopen() function need to give a relative path, otherwise it will find the file in the library function first
  */
  /*
    dlopen() is a function to load a dynamically loadable module(.so file). So variable tag needed to be stored, tag is pointed to the dlopen file.
  */
  handle = dlopen(strcat(relative_path, pathname), RTLD_NOW);
  if (!handle)
  {
    return NULL;
  }

  /* Initialize jitc */
  j = (struct jitc *)malloc(sizeof(struct jitc));
  if (!j)
  {
    dlclose(handle);
    return NULL;
  }

  /* store the tag */
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
    /* Free memory */
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

/* void *dlsym(void *handle, const char *symbol); */
long jitc_lookup(struct jitc *jitc, const char *symbol)
{
  void *sym_address = NULL;

  if (!jitc || !jitc->handle)
  {
    return 0;
  }

  /*
    Find the symbol named "evaluate" to get the address of the function,
    and convert the address to the correct function pointer type and assign it to fnc
  */
  sym_address = (char *)dlsym(jitc->handle, symbol);

  return (long)sym_address;
}
