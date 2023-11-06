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
  int fd;       /* 文件描述符 */
  void *memory; /*  内存映射的基地址 */
  size_t size;  /* 映射大小 */
  size_t used;  /* 已用空间 */
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
  size_t size = 0;
  size_t used = 0;

  printf("scm_open begin\n");

  /* 检查参数 */
  if (pathname == NULL)
  {
    return NULL;
  }

  printf("Already check pathname: %s\n", pathname);

  /* 分配scm结构体 */
  if (scm == NULL)
  {
    close(fd);
    return NULL;
  }

  printf("Already malloc scm\n");

  /* 打开文件 */
  fd = open(pathname, O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1)
  {
    free(scm);
    return NULL;
  }

  printf("Already open file\n");

  /* 获取文件大小 */
  if (fstat(fd, &statbuf) == -1)
  {
    close(fd);
    free(scm);
    return NULL;
  }

  printf("Already get file size\n");

  /* 检查文件是否为普通文件 */
  if (!S_ISREG(statbuf.st_mode))
  {
    close(fd);
    free(scm);
    return NULL;
  }

  printf("Already check file type\n");

  size = statbuf.st_size;

  printf("size: %ld\n", size);

  if (!truncate)
  {
    /* Call sbrk(0) to find the current break point. */
    void *current_break = sbrk(0);
    off_t end;
    if (current_break == (void *)-1)
    {
      close(fd);
      free(scm);
      return NULL;
    }

    if (current_break < (void *)VIRT_ADDR)
    {
      sbrk((char *)VIRT_ADDR - (char *)current_break);
    }

    printf("current_break: %p\n", current_break);

    /* Now allocate memory with mmap starting from VIRT_ADDR. */
    memory = mmap((void *)VIRT_ADDR, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED)
    {
      close(fd);
      free(scm);
      return NULL;
    }

    end = lseek(fd, -sizeof(size_t), SEEK_END);

    if (end != (off_t)-1)
    {
      if (read(fd, &used, sizeof(size_t)) == -1)
      {
        close(fd);
        return NULL;
      }
    }

    printf("used: %ld\n", used);

    printf("no-truncate end\n");
  }
  else
  {
    memory = mmap((void *)VIRT_ADDR, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED)
    {
      close(fd);
      free(scm);
      return NULL;
    }

    printf("truncate end\n");
  }

  printf("Already mmap file\n");

  /* 初始化scm结构体 */
  scm->fd = fd;
  scm->memory = memory;
  scm->size = size;
  scm->used = used;

  printf("memory: %p\n", memory);
  printf("size: %ld\n", size);
  printf("used: %ld\n", used);

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
  printf("scm_close begin\n");
  if (scm != NULL)
  {
    /* 将 used 的值写入文件的最后 */
    if (lseek(scm->fd, 0, SEEK_END) != (off_t)-1)
    {
      if (write(scm->fd, &(scm->used), sizeof(scm->used)) == -1)
      {
        close(scm->fd);
        return;
      }
    }

    /* 如果文件大小为0，使用munmap释放内存 */
    if (scm->size == 0)
    {
      munmap(scm->memory, scm->size);
    }
    else
    {
      /* 如果文件大小不为0，使用 msync 同步内存到文件 */
      msync(scm->memory, scm->size, MS_SYNC);
    }

    /* 关闭文件 */
    close(scm->fd);

    /* 释放scm结构体 */
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

  printf("scm_malloc begin\n");

  if (scm == NULL || n == 0 || scm->used + n > scm->size)
  {
    return NULL;
  }

  printf("Check passed\n");

  memory = (char *)scm->memory + scm->used;
  scm->used = scm->used + n;

  printf("memory: %p\n", memory);

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

  printf("scm_strdup begin\n");

  if (scm == NULL || s == NULL)
  {
    return NULL;
  }

  /* 计算字符串长度 */
  n = strlen(s) + 1;

  if (scm->used + n > scm->size)
  {
    return NULL;
  }

  memory = scm_malloc(scm, n);
  if (memory == NULL)
  {
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
  printf("scm_free begin\n");

  if (scm == NULL || p == NULL)
  {
    return;
  }

  /* 如果释放的内存地址在已用内存的末尾，更新已用内存大小 */
  if (p == (char *)scm->memory + scm->used)
  {
    scm->used = scm->used - sizeof(p);
  }

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
  printf("scm_utilized begin\n");
  if (scm == NULL)
  {
    return 0;
  }
  return scm->used;
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
  printf("scm_capacity begin\n");
  if (scm == NULL)
  {
    return 0;
  }
  return scm->size;
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
  printf("scm_mbase begin\n");
  if (scm == NULL)
  {
    return 0;
  }
  return scm->memory;
}
