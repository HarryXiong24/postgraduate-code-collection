/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * logfs.c
 */

#define _POSIX_C_SOURCE 200112L
#include <pthread.h>
#include "device.h"
#include "logfs.h"
#include <stdlib.h>

#define WCACHE_BLOCKS 32
#define RCACHE_BLOCKS 256

/**
 * Needs:
 *   pthread_create()
 *   pthread_join()
 *   pthread_mutex_init()
 *   pthread_mutex_destroy()
 *   pthread_mutex_lock()
 *   pthread_mutex_unlock()
 *   pthread_cond_init()
 *   pthread_cond_destroy()
 *   pthread_cond_wait()
 *   pthread_cond_signal()
 */

/* research the above Needed API and design accordingly */

#define BLOCK_SIZE 1024

struct cacheBlock
{
    char data[BLOCK_SIZE];
    uint64_t off; /* 表示数据在缓存中的索引*/
    int isUsed;   /* 用于指示该缓存块是否已被使用 */
};

struct cache
{
    struct cacheBlock blocks[RCACHE_BLOCKS];
};

/*
 * writeBuffer 主要用于收集和暂存即将写入设备的数据。它充当一个临时存储区，用于合并较小的写入请求，以便可以以更大的块写入底层设备。
 * 这个缓冲区可能会在收集到足够的数据以填满一个块时触发实际的写入操作。这种方法有助于减少写入操作的次数，提高I / O效率。
 */
struct appendQueue
{
    void *buf;
    uint64_t head;
    uint64_t tail;
    uint64_t size;
};

/*
 * appendBuffer 用于管理已经准备好写入设备的数据。这个缓冲区通常比 writeBuffer 大，并且用于存储从 writeBuffer 迁移过来的数据。
 * 当 writeBuffer 被填满并准备好写入时，数据会被传输到 appendBuffer。appendBuffer 起到了一个中转站的作用，保存数据直到被后台写入线程处理。
 */
struct writeQueue
{
    void *buf;
    uint64_t size;
    uint64_t utilized;
};

pthread_mutex_t mutex;
pthread_cond_t cond;

struct appendQueue appendBuffer;
int isInit = 0; /* appendBuffer 是否初始化 */
int isExit = 0; /* 用于指示是否退出后台写入线程 */

struct writeQueue writeBuffer = {NULL, BLOCK_SIZE, 0};
struct cache cache;

pthread_t consumerThread;

struct logfs
{
    struct device *device;
    uint64_t tail; /* 用于追踪数据写入的位置 */
    uint64_t tmp;
};

void *mallocSpace(int n)
{
    void *buf = NULL;
    int result = posix_memalign((void **)&buf, 4096, BLOCK_SIZE * n); /* 尝试以 4096 字节对齐的方式分配内存，它分配 n 个 BLOCK_SIZE 大小的内存块 */
    if (result == 0)
    {
        memset(buf, 0, BLOCK_SIZE * n);
        return buf;
    }
    else
    {
        return NULL;
    }
}

void *consumer(void *arg)
{
    struct logfs *logfs = (struct logfs *)arg;
    void *writeBuf = mallocSpace(1); /* 临时存储要写入设备的数据 */
    uint64_t index = 0;

    while (1)
    {
        if (isExit == 1)
        {
            break;
        }
        pthread_mutex_lock(&mutex);
        /*
         * 函数检查 appendBuffer 中是否有数据需要写入（通过比较 savedTail 和 currentTail）
         * 如果有，函数将数据从 appendBuffer 复制到临时缓冲区 writBuff，然后调用 device_write 将其写入设备。*/
        if (appendBuffer.tail != appendBuffer.head)
        {
            if (appendBuffer.tail >= appendBuffer.size)
            {
                appendBuffer.tail = 0;
            }
            memcpy(writeBuf, (char *)appendBuffer.buf + appendBuffer.tail, BLOCK_SIZE);
            if (device_write(logfs->device, writeBuf, logfs->tail, BLOCK_SIZE) == -1)
            {
                printf("device_write error\n");
                break;
            }
            /*
             * 更新 logfs 结构中的 tail，表示数据已被追加到设备的末尾
             * 同时，appendBuffer 的 tail 也要被更新，以准备下一轮写入 */
            index = (logfs->tail) % RCACHE_BLOCKS;
            memcpy(cache.blocks[index].data, writeBuf, BLOCK_SIZE);
            cache.blocks[index].off = logfs->tail;
            cache.blocks[index].isUsed = 1;
            logfs->tail += BLOCK_SIZE;
            appendBuffer.tail += BLOCK_SIZE;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            if (isExit == 1)
            {
                break;
            }
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
    free(writeBuf);
    pthread_exit(NULL);
    return NULL;
}

/**
 * Opens the block device specified in pathname for buffered I/O using an
 * append only log structure.
 *
 * pathname: the pathname of the block device
 *
 * return: an opaque handle or NULL on error
 */
struct logfs *
logfs_open(const char *pathname)
{
    struct logfs *new_logfs;
    struct device *device;

    new_logfs = (struct logfs *)malloc(sizeof(struct logfs));
    if (new_logfs == NULL)
    {
        return NULL;
    }
    memset(new_logfs, 0, sizeof(struct logfs));

    device = device_open(pathname);
    if (device == NULL)
    {
        return NULL;
    }

    new_logfs->device = device;
    new_logfs->tail = 0;
    new_logfs->tmp = 0;

    /* init appendBuffer */
    memset(&appendBuffer, 0, sizeof(struct appendQueue));
    appendBuffer.buf = mallocSpace(WCACHE_BLOCKS);
    appendBuffer.head = 0;
    appendBuffer.tail = 0;
    appendBuffer.size = BLOCK_SIZE * WCACHE_BLOCKS;

    /* init writeBuffer */
    memset(&writeBuffer, 0, sizeof(struct writeQueue));
    writeBuffer.buf = mallocSpace(1);
    writeBuffer.utilized = 0;
    writeBuffer.size = BLOCK_SIZE;
    isInit = 1;

    /* init cache */
    memset(&cache, 0, sizeof(struct cache));
    isExit = 0;

    /* init thread */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_create(&consumerThread, NULL, consumer, (void *)new_logfs);

    return new_logfs;
}

/**
 * Closes a previously opened logfs handle.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 *
 * Note: logfs may be NULL.
 */
void logfs_close(struct logfs *logfs)
{
    if (logfs == NULL)
    {
        return;
    }

    pthread_mutex_lock(&mutex);

    if (writeBuffer.size != 0 && writeBuffer.buf != NULL)
    {
        if (appendBuffer.head == appendBuffer.size)
        {
            appendBuffer.head = 0;
        }
        memcpy((char *)appendBuffer.buf + appendBuffer.head, writeBuffer.buf, BLOCK_SIZE);
    }

    isExit = 1;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
    pthread_join(consumerThread, NULL);
    device_close(logfs->device);

    free(appendBuffer.buf);
    free(writeBuffer.buf);
    free(logfs);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    memset(&cache, 0, sizeof(struct cache));

    return;
}

/* Calculate the offset into the block */
uint64_t calculateAlignOffset(uint64_t off)
{
    off -= off % BLOCK_SIZE;
    return off;
}

/* Calculate the offset into the block */
size_t calculateAlignLen(uint64_t off, size_t len)
{
    len = ((len + off) % BLOCK_SIZE) == 0 ? (len + off) / BLOCK_SIZE : ((len + off) / BLOCK_SIZE) + 1;
    return len;
}

/**
 * Random read of len bytes at location specified in off from the logfs.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 * buf  : a region of memory large enough to receive len bytes
 * off  : the starting byte offset
 * len  : the number of bytes to read
 *
 * return: 0 on success, otherwise error
 */
int logfs_read(struct logfs *logfs, void *buf, uint64_t off, size_t len)
{
    uint64_t alignOff;
    size_t alignLen;
    void *tempBuffer;
    uint64_t i;
    uint64_t updateCacheNeeded = 0;

    if (logfs == NULL || isInit == 0)
    {
        return -1;
    }

    alignLen = calculateAlignLen(off, len);
    alignOff = calculateAlignOffset(off);

    /* 分配一个临时缓冲区 tempBuffer */
    tempBuffer = mallocSpace(alignLen);
    if (tempBuffer == NULL)
    {
        return -1;
    }

    /* make sure everything we need is write into the device */
    /*
     * 确保要读取的数据已经被写入设备
     * 如果 logfs->tailIndex 小于所需数据的末尾位置，或者 appendBuffer 中还有未保存的数据，会等待或触发写入操作
     */
    while ((logfs->tail < alignOff + BLOCK_SIZE * alignLen) && (appendBuffer.head != appendBuffer.tail))
    {
        pthread_cond_signal(&cond);
    }

    /* 如果 writeBuffer 中有未处理的数据，它将被写入设备以确保读取操作可以获得最新的数据 */
    if (writeBuffer.size != 0)
    {
        uint64_t index = (logfs->tmp) % RCACHE_BLOCKS;
        device_write(logfs->device, writeBuffer.buf, logfs->tmp, BLOCK_SIZE);
        memcpy(cache.blocks[index].data, writeBuffer.buf, BLOCK_SIZE);
        cache.blocks[index].off = logfs->tmp;
        cache.blocks[index].isUsed = 1;
    }

    /*
     * 函数尝试从缓存（cache）中读取数据
     * 如果所需数据在缓存中未命中或缓存不包含最新数据，则从设备中读取整个对齐的块
     */
    for (i = 0; i < alignLen; i++)
    {
        /* get the offset */
        uint64_t index = (alignOff / BLOCK_SIZE + i) % RCACHE_BLOCKS;
        /* not hit the cache */
        if (cache.blocks[index].isUsed == 0 || cache.blocks[index].off != alignOff + i * BLOCK_SIZE)
        {
            device_read(logfs->device, tempBuffer, alignOff, alignLen * BLOCK_SIZE);
            /* update the buffer into the cache */
            updateCacheNeeded = 1;
            break;
        }
        else
        {
            memcpy((char *)tempBuffer + i * BLOCK_SIZE, cache.blocks[index].data, BLOCK_SIZE);
        }
    }
    memcpy(buf, (char *)tempBuffer + (off - alignOff), len);

    /* update the cache if needed */
    if (updateCacheNeeded == 1)
    {
        for (i = 0; i < 1; i++)
        {
            uint64_t index = (alignOff / BLOCK_SIZE + i) % RCACHE_BLOCKS;
            memcpy(cache.blocks[index].data, (char *)tempBuffer + i * BLOCK_SIZE, BLOCK_SIZE);
            cache.blocks[index].off = alignOff + i * BLOCK_SIZE;
            cache.blocks[index].isUsed = 1;
        }
    }

    free(tempBuffer);

    return 0;
}

void *writeDataToWriteBuffer(const void *data, size_t *len)
{
    if (writeBuffer.buf == NULL || len == NULL || *len <= 0 || data == NULL)
    {
        return NULL;
    }

    if (writeBuffer.utilized + *len > writeBuffer.size)
    {
        /* 如果新数据的长度加上当前缓冲区的大小超过了缓冲区的最大容量，函数只会复制能够放入剩余空间的部分数据。剩余未写入的数据长度通过减少 *len 来更新 */
        size_t available = writeBuffer.size - writeBuffer.utilized;
        memcpy((char *)writeBuffer.buf + writeBuffer.utilized, data, available);
        *len -= available;
        writeBuffer.utilized = 0;
        return (void *)((char *)data + available);
    }
    else if (writeBuffer.utilized + *len == writeBuffer.size)
    {
        /* 如果加入新数据后缓冲区恰好被填满，函数会将所有数据复制到缓冲区，并将 *len 设置为 0，表示所有数据都已经被处理 */
        size_t available = writeBuffer.size - writeBuffer.utilized;
        memcpy((char *)writeBuffer.buf + writeBuffer.utilized, data, available);
        *len = 0;
        writeBuffer.utilized = 0;
        return (void *)("CanBeWrite");
    }
    /* 如果新数据没有填满缓冲区，那么这部分数据会被复制到缓冲区，并更新 writeBuffer.curSize（当前缓冲区大小）以反映新增的数据量 */
    memcpy((char *)writeBuffer.buf + writeBuffer.utilized, data, *len);
    writeBuffer.utilized += *len;
    return NULL;
}

/**
 * Append len bytes to the logfs.
 *
 * logfs: an opaque handle previously obtained by calling logfs_open()
 * buf  : a region of memory holding the len bytes to be written
 * len  : the number of bytes to write
 *
 * return: 0 on success, otherwise error
 */
int logfs_append(struct logfs *logfs, const void *buf, uint64_t len)
{
    void *wordChecker;
    int count = 0;

    if (logfs == NULL || writeBuffer.buf == NULL || isInit == 0)
    {
        return -1;
    }
    /* 没有数据需要写入，函数直接返回成功 0 */
    if (len == 0)
    {
        return 0;
    }

    /* 确保同时只有一个线程可以修改缓冲区 */
    pthread_mutex_lock(&mutex);
    /* 调用 writeDataToWriteBuffer 函数将数据写入 writeBuffer */
    wordChecker = writeDataToWriteBuffer(buf, &len);
    /* 如果 writeToBuffer 返回非 NULL，这表示有未写入的数据，则需要将剩余数据复制到 appendBuffer 并更新相关索引，直到所有数据都被处理*/
    while (wordChecker != NULL)
    {
        count++;
        if (appendBuffer.head == appendBuffer.size)
        {
            appendBuffer.head = 0;
        }
        memcpy((char *)appendBuffer.buf + appendBuffer.head, writeBuffer.buf, BLOCK_SIZE);
        appendBuffer.head += BLOCK_SIZE;
        logfs->tmp += BLOCK_SIZE;
        memset(writeBuffer.buf, 0, BLOCK_SIZE);

        wordChecker = writeDataToWriteBuffer(wordChecker, &len);

        if (wordChecker == (void *)("CanBeWrite"))
        {
            if (appendBuffer.head == appendBuffer.size)
            {
                appendBuffer.head = 0;
            }
            memcpy((char *)appendBuffer.buf + appendBuffer.head, writeBuffer.buf, BLOCK_SIZE);
            appendBuffer.head += BLOCK_SIZE;
            logfs->tmp += BLOCK_SIZE;
            memset(writeBuffer.buf, 0, BLOCK_SIZE);
            wordChecker = writeDataToWriteBuffer(wordChecker, &len);
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    if (count != 0)
    {
        pthread_cond_signal(&cond);
    }
    return 0;
}
