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
  int fd = -1;
  void *memory = NULL;
  size_t capacity = 0;
  size_t utilized = 0;

  if (pathname == NULL)
  {
    TRACE("Error");
    return NULL;
  }

  if (scm == NULL)
  {
    TRACE("Error");
    close(fd);
    return NULL;
  }

  fd = open(pathname, O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1)
  {
    TRACE("Error");
    free(scm);
    return NULL;
  }

  /* get file size */
  if (fstat(fd, &statbuf) == -1)
  {
    TRACE("Error");
    close(fd);
    free(scm);
    return NULL;
  }

  /* check file type */
  if (!S_ISREG(statbuf.st_mode))
  {
    TRACE("Error");
    close(fd);
    free(scm);
    return NULL;
  }

  /* if (sbrk(scm->capacity) == (void *)-1)
  {
    TRACE("Error");
    close(fd);
    free(scm);
    return NULL;
  } */

  capacity = statbuf.st_size;

  memory = mmap((void *)VIRT_ADDR, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (memory == MAP_FAILED)
  {
    TRACE("Error");
    close(fd);
    free(scm);
    return NULL;
  }

  if (!truncate)
  {
    if (lseek(fd, -sizeof(size_t), SEEK_END) != (off_t)-1)
    {
      if (read(fd, &utilized, sizeof(size_t)) == -1)
      {
        TRACE("Error");
        close(fd);
        return NULL;
      }
    }
  }

  /* initialize scm */
  scm->fd = fd;
  scm->memory = memory;
  scm->capacity = capacity;
  scm->utilized = utilized;

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
    if (lseek(scm->fd, -sizeof(size_t), SEEK_END) != (off_t)-1)
    {
      if (write(scm->fd, &(scm->utilized), sizeof(scm->utilized)) == -1)
      {
        TRACE("Error");
        close(scm->fd);
        return;
      }
    }

    if (msync(scm->memory, scm->utilized, MS_SYNC) == -1)
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
  void *memory = NULL;

  if (scm == NULL || n == 0 || scm->utilized + n > scm->capacity)
  {
    TRACE("Error");
    return NULL;
  }

  memory = (char *)scm->memory + scm->utilized;
  scm->utilized = scm->utilized + n;

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

  /* 计算字符串长度 */
  n = strlen(s) + 1;

  if (scm->utilized + n > scm->capacity)
  {
    return NULL;
  }

  memory = scm_malloc(scm, n);
  if (memory == NULL)
  {
    TRACE("Error");
    return NULL;
  }

  strncpy(memory, s, n);

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
  if (scm == NULL || p == NULL)
  {
    TRACE("Error");
    return;
  }

  scm->utilized = scm->utilized - (size_t)sizeof(p);

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

/**？。
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
  return scm->memory;
}
