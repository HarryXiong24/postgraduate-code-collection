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

/*
 * appendBuffer 用于管理已经准备好写入 device 的数据
 * 这个缓冲区通常比 writeBuffer 大，并且用于存储从 writeBuffer 迁移过来的数据
 * 当 writeBuffer 被填满并准备好写入时，数据会被传输到 appendBuffer。appendBuffer 起到了一个中转站的作用，保存数据直到被后台写入
 */
struct appendQueue
{
    void *address;
    uint64_t head; /* 表示可以从 appendQueue 中读取数据的位置 */
    uint64_t tail; /* 表示 appendQueue 中已经有数据的 block 中最末尾位置 */
    uint64_t size;
};

/*
 * writeBuffer 主要用于收集和暂存即将写入 device 的数据
 * 它充当一个临时存储区，用于合并较小的写入请求，以便可以以更大的块写入底层设备
 * 这个缓冲区可能会在收集到足够的数据以填满一个块时触发实际的写入操作。这种方法有助于减少写入操作的次数，提高I/O效率
 */
struct writeQueue
{
    void *address;
    uint64_t size;
    uint64_t utilized;
};

/* 缓存结构体定义，在 read 中会优先读取缓存里的数据，减少对 device 的访问次数，加快数据读取速度 */
struct cache
{
    struct cacheBlock
    {
        char data[BLOCK_SIZE];
        uint64_t off; /* 表示数据在缓存中的索引*/
        int isUsed;   /* 用于表示该缓存块是否已被使用 */
    } blocks[RCACHE_BLOCKS];
};

struct logfs
{
    struct device *device;
    uint64_t tail; /* 指向下一个将要写入数据的位置 */
    uint64_t head; /* 指向最早写入数据的位置 */
};

pthread_mutex_t mutex;
pthread_cond_t cond;

struct appendQueue appendBuffer;
int isInit = 0; /* appendBuffer 是否初始化 */
int isExit = 0; /* 用于指示是否退出后台写入线程 */

struct writeQueue writeBuffer;

struct cache cache;

pthread_t consumerThread;

/* malloc memory */
void *mallocAlignSpace(int n)
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

/* 从 appendBuffer 中读取数据并将其写入到 device */
void *consumer(void *arg)
{
    struct logfs *logfs = (struct logfs *)arg;
    void *tempBuf = mallocAlignSpace(1); /* 临时存储要写入设备的数据 */
    uint64_t index = 0;

    while (1)
    {
        if (isExit == 1)
        {
            break;
        }
        pthread_mutex_lock(&mutex);
        /* 检查 appendBuffer 中是否有数据需要写入 */
        if (appendBuffer.tail != appendBuffer.head)
        {
            if (appendBuffer.tail >= appendBuffer.size)
            {
                appendBuffer.tail = 0;
            }
            /* 如果有，将数据从 appendBuffer 复制到临时缓冲区 writBuff，然后调用 device_write 将其写入设 备 */
            memcpy(tempBuf, (char *)appendBuffer.address + appendBuffer.tail, BLOCK_SIZE);
            /* 使用 device_write 将数据写入设备 */
            if (device_write(logfs->device, tempBuf, logfs->tail, BLOCK_SIZE) == -1)
            {
                printf("device_write error\n");
                break;
            }
            /* 将写入的数据复制到缓存中 */
            index = (logfs->tail) % RCACHE_BLOCKS;
            memcpy(cache.blocks[index].data, tempBuf, BLOCK_SIZE);
            cache.blocks[index].off = logfs->tail;
            cache.blocks[index].isUsed = 1;

            /*
             * 更新 logfs 结构中的 tail，表示数据已被追加到 device 的末尾
             * 同时，appendBuffer 的 tail 也要被更新，以准备下一轮写入
             */
            logfs->tail += BLOCK_SIZE;
            appendBuffer.tail += BLOCK_SIZE;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            /* 如果 appendBuffer 中没有可用数据，则通过 pthread_cond_wait 进入等待 */
            if (isExit == 1)
            {
                break;
            }
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
    free(tempBuf);
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
    new_logfs->head = 0;

    /* init appendBuffer */
    memset(&appendBuffer, 0, sizeof(struct appendQueue));
    appendBuffer.address = mallocAlignSpace(WCACHE_BLOCKS);
    appendBuffer.head = 0;
    appendBuffer.tail = 0;
    appendBuffer.size = WCACHE_BLOCKS * BLOCK_SIZE;

    /* init writeBuffer */
    memset(&writeBuffer, 0, sizeof(struct writeQueue));
    writeBuffer.address = mallocAlignSpace(1);
    writeBuffer.utilized = 0;
    writeBuffer.size = BLOCK_SIZE;

    /* init cache */
    memset(&cache, 0, sizeof(struct cache));

    /* init status */
    isInit = 1;
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
/* 确保所有挂起的写入操作都完成，并且释放所有分配的资源 */
void logfs_close(struct logfs *logfs)
{
    if (logfs == NULL)
    {
        return;
    }

    pthread_mutex_lock(&mutex);

    /*
     * 检查 writeBuffer 是否有未处理的数据
     * 如果有，将它们复制到 appendBuffer，准备写入
     */
    if (writeBuffer.utilized != 0 && writeBuffer.address != NULL)
    {
        if (appendBuffer.head == appendBuffer.size)
        {
            appendBuffer.head = 0;
        }
        memcpy((char *)appendBuffer.address + appendBuffer.head, writeBuffer.address, BLOCK_SIZE);
    }

    isExit = 1; /* 通知后台写入线程退出 */
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);         /* 唤醒可能处于等待状态的 consumer 线程 */
    pthread_join(consumerThread, NULL); /* 等待 consumer 线程完成，确保所有数据都被正确处理 */
    device_close(logfs->device);

    free(appendBuffer.address);
    free(writeBuffer.address);
    free(logfs);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    memset(&cache, 0, sizeof(struct cache));

    return;
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
    uint64_t i;
    uint64_t isUpdateCache = 0;

    uint64_t alignOff = off - off % BLOCK_SIZE;                                                                    /* 偏移量会小于或等于原始偏移量，最接近的块边界，这确保了读取操作从一个完整的块开始 */
    size_t alignLen = ((len + off) % BLOCK_SIZE) == 0 ? (len + off) / BLOCK_SIZE : ((len + off) / BLOCK_SIZE) + 1; /* 对齐后块的数量，如果原始长度和偏移量的总和不是块大小的整数倍，则需要多读取一个块以覆盖所有数据*/

    void *tempBuffer = mallocAlignSpace(alignLen);
    if (tempBuffer == NULL)
    {
        return -1;
    }

    if (logfs == NULL || isInit == 0)
    {
        return -1;
    }

    /*
     * 确保要读取的数据已经被写入设备
     * 如果 logfs->tailIndex 小于所需数据的末尾位置，或者 appendBuffer 中还有未保存的数据，会等待或触发写入操作
     */
    while ((logfs->tail < alignOff + BLOCK_SIZE * alignLen) && (appendBuffer.head != appendBuffer.tail))
    {
        pthread_cond_signal(&cond);
    }

    /* 如果 writeBuffer 中有未处理的数据，它将被写入设备以确保读取操作可以获得最新的数据 */
    if (writeBuffer.utilized != 0)
    {
        uint64_t index = (logfs->head) % RCACHE_BLOCKS;
        device_write(logfs->device, writeBuffer.address, logfs->head, BLOCK_SIZE);
        memcpy(cache.blocks[index].data, writeBuffer.address, BLOCK_SIZE);
        cache.blocks[index].off = logfs->head;
        cache.blocks[index].isUsed = 1;
    }

    /*
     * 函数尝试从缓存（cache）中读取数据
     * 如果所需数据在缓存中未命中或缓存不包含最新数据，则从设备中读取整个对齐的块
     */
    for (i = 0; i < alignLen; i++)
    {
        uint64_t blockIndex = (alignOff / BLOCK_SIZE + i);
        uint64_t cacheIndex = blockIndex % RCACHE_BLOCKS;
        int isCacheHit = cache.blocks[cacheIndex].isUsed && cache.blocks[cacheIndex].off == blockIndex;

        /* 没有在 cache 中找到 */
        if (!isCacheHit)
        {
            device_read(logfs->device, tempBuffer, alignOff, alignLen * BLOCK_SIZE);
            /* update the buffer into the cache */
            isUpdateCache = 1;
            break;
        }
        else
        {
            /* 在 cache 中找到 */
            memcpy((char *)tempBuffer + i * BLOCK_SIZE, cache.blocks[cacheIndex].data, BLOCK_SIZE);
        }
    }
    memcpy(buf, (char *)tempBuffer + (off - alignOff), len);

    if (isUpdateCache == 1)
    {
        for (i = 0; i < alignLen; i++)
        {
            uint64_t cacheIndex = (alignOff / BLOCK_SIZE + i) % RCACHE_BLOCKS;
            memcpy(cache.blocks[cacheIndex].data, (char *)tempBuffer + i * BLOCK_SIZE, BLOCK_SIZE);
            cache.blocks[cacheIndex].off = alignOff + i * BLOCK_SIZE;
            cache.blocks[cacheIndex].isUsed = 1;
        }
    }

    free(tempBuffer);

    return 0;
}

/* 用于将 writeBuffer 数据刷新到 appendBuffer */
void flushWriteBufferToAppendBuffer(struct logfs *logfs)
{
    if (appendBuffer.head + BLOCK_SIZE > appendBuffer.size)
    {
        appendBuffer.head = 0;
    }
    memcpy((char *)appendBuffer.address + appendBuffer.head, writeBuffer.address, BLOCK_SIZE);
    appendBuffer.head += BLOCK_SIZE;
    logfs->head += BLOCK_SIZE;

    /* reset */
    memset(writeBuffer.address, 0, BLOCK_SIZE);
    writeBuffer.utilized = 0;
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
    if (logfs == NULL || writeBuffer.address == NULL || isInit == 0)
    {
        return -1;
    }

    if (len == 0)
    {
        return 0;
    }

    pthread_mutex_lock(&mutex);

    while (len > 0)
    {
        size_t spaceAvailable = writeBuffer.size - writeBuffer.utilized;
        size_t toCopy = len < spaceAvailable ? len : spaceAvailable;

        memcpy((char *)writeBuffer.address + writeBuffer.utilized, buf, toCopy);
        writeBuffer.utilized += toCopy;
        buf = (const char *)buf + toCopy;
        len -= toCopy;

        if (writeBuffer.utilized == writeBuffer.size)
        {
            flushWriteBufferToAppendBuffer(logfs); /* 用于将 writeBuffer 数据刷新到 appendBuffer */
        }
    }

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond); /* 通知可能等待的消费者线程 */

    return 0;
}
