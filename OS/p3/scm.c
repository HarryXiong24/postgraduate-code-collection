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
#include "system.h"

#define VIRT_ADDR 0x600000000000

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

/* research the above Needed API and design accordingly */

struct scm
{
  int fd;          /* file descriptor */
  void *memory;    /* base memory address */
  size_t capacity; /* total memory */
  size_t utilized; /* utilized memory */
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
  struct scm *scm = (struct scm *)malloc(sizeof(struct scm));
  struct stat statbuf;

  if (pathname == NULL)
  {
    TRACE("Error");
    return NULL;
  }

  if (scm == NULL)
  {
    TRACE("Error");
    return NULL;
  }

  scm->fd = open(pathname, O_RDWR, S_IRUSR | S_IWUSR);
  if (scm->fd == -1)
  {
    TRACE("Error");
    free(scm);
    return NULL;
  }

  /* get file size */
  if (fstat(scm->fd, &statbuf) == -1)
  {
    TRACE("Error");
    close(scm->fd);
    free(scm);
    return NULL;
  }

  /* check file type */
  if (!S_ISREG(statbuf.st_mode))
  {
    TRACE("Error");
    close(scm->fd);
    free(scm);
    return NULL;
  }

  scm->capacity = statbuf.st_size;

  scm->memory = mmap((void *)VIRT_ADDR, scm->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, scm->fd, 0);
  if (scm->memory == MAP_FAILED)
  {
    TRACE("Error");
    close(scm->fd);
    free(scm);
    return NULL;
  }

  if (!truncate)
  {
    /* if not truncate, read the utilized in the header, use size_t space */
    scm->utilized = *(size_t *)scm->memory;
  }
  else
  {
    /* if truncate, store the utilized in the header, use size_t space */
    scm->utilized = 0;
    *(size_t *)scm->memory = scm->utilized;
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
    if (msync(scm->memory, scm->capacity, MS_SYNC) == -1)
    {
      TRACE("Error");
    }

    if (munmap(scm->memory, scm->capacity) == -1)
    {
      TRACE("Error");
    }

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
  void *memory;
  size_t *block_size;

  if (scm == NULL || n == 0)
  {
    TRACE("Error");
    return NULL;
  }

  if (scm->utilized + n + sizeof(size_t) > scm->capacity)
  {
    TRACE("Error");
    return NULL;
  }

  /* calculate the position of store the size */
  block_size = (size_t *)((char *)scm->memory + sizeof(size_t) + scm->utilized);

  /* store the size */
  *block_size = n;

  /* move the pointer to the actual start of the allocated block */
  memory = (void *)(block_size + 1);
  scm->utilized += n + sizeof(size_t);

  /* update the memory header to store the new utilized value */
  *(size_t *)scm->memory = scm->utilized;

  return memory;
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
  char *memory = NULL;
  size_t n = 0;

  if (scm == NULL || s == NULL)
  {
    TRACE("Error");
    return NULL;
  }

  /* need to include '\0' */
  n = strlen(s) + 1;

  memory = scm_malloc(scm, n);
  if (memory == NULL)
  {
    TRACE("Error");
    return NULL;
  }

  memcpy(memory, s, n);

  return memory;
}

/**
 * Analogous to the standard C free function, but using SCM region.
 *
 * scm: an opaque handle previously obtained by calling scm_open()
 * p  : a pointer to the start of a previously allocated memory
 */
void scm_free(struct scm *scm, void *p)
{
  size_t *size_ptr;
  size_t size;

  if (scm == NULL || p == NULL)
  {
    TRACE("Error");
    return;
  }

  size_ptr = (size_t *)((char *)p - sizeof(size_t));
  size = *size_ptr;

  /* update utilized */
  scm->utilized = scm->utilized - size - sizeof(size_t);

  /* update the header information */
  *(size_t *)scm->memory = scm->utilized;

  return;
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
  if (scm == NULL)
  {
    TRACE("Error");
    return 0;
  }
  return scm->utilized;
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
  if (scm == NULL)
  {
    TRACE("Error");
    return 0;
  }
  return scm->capacity;
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
  if (scm == NULL)
  {
    TRACE("Error");
    return 0;
  }

  /* skip the space used to store the utilized size  */
  return (char *)scm->memory + sizeof(size_t) + sizeof(size_t);
}
