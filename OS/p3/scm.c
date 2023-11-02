/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * scm.c
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "scm.h"

/**
 * Needs:
 *   fstat()
 *   S_ISREG()
 *   open()
 *   close()
 *   sbrk()
 *   mmap()
 *   munmap()
 *   msync()
 */

/* VIRT_ADDR OX600000000000 */
/* open(pathname, mode) -> fd */
/* close(fd) */
/* fstat(fd) */
/* S_ISREG() */
/* mmap(VIRT_ADDR, length, PORT_READ | PORT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0) -> Addr */

/* research the above Needed API and design accordingly */

struct scm
{
  int fd;
  size_t length;
  size_t size;
  void *addr;
};

/**
 * Initializes an SCM region using the file specified in pathname as the
 * backing device, opening the regsion for memory allocation activities.
 *
 * pathname: the file pathname of the backing device
 * truncate: if non-zero, truncates the SCM region, clearning all data
 *
 * return: an opaque handle or NULL on error
 */

struct scm *scm_open(const char *pathname, int truncate)
{
  struct scm *scm = malloc(sizeof(struct scm));
  if (scm == NULL)
  {
    return NULL;
  }
  int fd = open(pathname, O_RDWR | O_CREAT, 0666);
  if (fd == -1)
  {
    free(scm);
    return NULL;
  }
  scm->fd = fd;
  scm->length = lseek(fd, 0, SEEK_END);
  if (truncate)
  {
    scm->size = 0;
  }
  else
  {
    scm->size = scm->length;
  }
  scm->addr = mmap(NULL, scm->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (scm->addr == MAP_FAILED)
  {
    close(fd);
    free(scm);
    return NULL;
  }
  return scm;
}

/**
 * Closes a previously opened SCM handle.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * Note: scm may be NULL
 */
void scm_close(struct scm *scm)
{
  if (scm != NULL)
  {
    close(scm->fd);
    free(scm);
  }
}

/**
 * Analogous to the standard C malloc function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * n  : the size of the requested memory in bytes
 *
 * return: a pointer to the start of the allocated memory or NULL on error
 */
void *scm_malloc(struct scm *scm, size_t n)
{
  if (scm->size + n > scm->length)
  {
    return NULL;
  }
  void *addr = scm->addr + scm->size;
  scm->size += n;
  return addr;
}

/**
 * Analogous to the standard C strdup function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * s  : a C string to be duplicated.
 *
 * return: the base memory address of the duplicated C string or NULL on error
 */
char *scm_strdup(struct scm *scm, const char *s)
{
  size_t n = strlen(s) + 1;
  char *addr = scm_malloc(scm, n);
  if (addr == NULL)
  {
    return NULL;
  }
  memcpy(addr, s, n);
  return addr;
}

/**
 * Analogous to the standard C free function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * p  : a pointer to the start of a previously allocated memory
 */
void scm_free(struct scm *scm, void *p)
{
  // do nothing
}

/**
 * Returns the number of SCM bytes utilized thus far.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * return: the number of bytes utilized thus far
 */
size_t scm_utilized(const struct scm *scm)
{
  return scm->size;
}

/**
 * Returns the number of SCM bytes available in total.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * return: the number of bytes available in total
 */
size_t scm_capacity(const struct scm *scm)
{
  return scm->length;
}

/**
 * Returns the base memory address within the SCM region, i.e., the memory
 * pointer that would have been returned by the first call to scm_malloc()
 * after a truncated initialization.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 *
 * return: the base memory address within the SCM region
 */
void *scm_mbase(struct scm *scm)
{
  return scm->addr;
}
