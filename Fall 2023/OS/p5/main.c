/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include <signal.h>
#include "system.h"

/**
 * Needs:
 *   signal()
 */

static volatile int done;

typedef struct
{
	unsigned long received_packets;
	unsigned long sent_packets;
} NetworkPackets;

typedef struct
{
	char device_name[128];
	unsigned long read_blocks;
	unsigned long written_blocks;
} BlockStats;

static void
_signal_(int signum)
{
	assert(SIGINT == signum);

	done = 1;
}

double
cpu_util(const char *s)
{
	static unsigned sum_, vector_[7];
	unsigned sum, vector[7];
	const char *p;
	double util;
	uint64_t i;

	/*
		user
		nice
		system
		idle
		iowait
		irq
		softirq
	*/

	if (!(p = strstr(s, " ")) ||
			(7 != sscanf(p,
									 "%u %u %u %u %u %u %u",
									 &vector[0],
									 &vector[1],
									 &vector[2],
									 &vector[3],
									 &vector[4],
									 &vector[5],
									 &vector[6])))
	{
		return 0;
	}
	sum = 0.0;
	for (i = 0; i < ARRAY_SIZE(vector); ++i)
	{
		sum += vector[i];
	}
	util = (1.0 - (vector[3] - vector_[3]) / (double)(sum - sum_)) * 100.0;
	sum_ = sum;
	for (i = 0; i < ARRAY_SIZE(vector); ++i)
	{
		vector_[i] = vector[i];
	}
	return util;
}

/* calculate memory util */
double memory_util()
{
	const char *const PROC_MEMINFO = "/proc/meminfo";
	char line[1024];
	FILE *file;
	unsigned long memTotal = 0, memFree = 0;
	double memUsage = 0.0;

	file = fopen(PROC_MEMINFO, "r");
	if (!file)
	{
		perror("fopen");
		return -1;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (strncmp(line, "MemTotal:", 9) == 0)
		{
			sscanf(line, "MemTotal: %lu kB", &memTotal);
		}
		if (strncmp(line, "MemFree:", 8) == 0)
		{
			sscanf(line, "MemFree: %lu kB", &memFree);
			break;
		}
	}

	fclose(file);

	if (memTotal > 0)
	{
		memUsage = 100.0 - ((memFree / (double)memTotal) * 100.0);
	}

	return memUsage;
}

NetworkPackets get_network_packets(const char *interface)
{
	const char *const PROC_NET_DEV = "/proc/net/dev";
	FILE *file;
	char line[1024];
	NetworkPackets packets = {0, 0};
	char network_interface[128];

	file = fopen(PROC_NET_DEV, "r");
	if (!file)
	{
		perror("fopen");
		return packets;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (sscanf(line, " %127[^:]: %*d %lu %*d %*d %*d %*d %*d %*d %*d %lu",
							 network_interface, &packets.received_packets, &packets.sent_packets) == 3)
		{
			if (strcmp(network_interface, interface) == 0)
			{
				break;
			}
		}
	}

	fclose(file);
	return packets;
}

/* get tcp count */
int get_tcp_connections()
{
	const char *const TCP = "/proc/net/tcp";
	FILE *file;
	char line[1024];
	int count = 0;

	file = fopen(TCP, "r");
	if (!file)
	{
		perror("fopen");
		return -1;
	}

	/* ignore title row */
	if (fgets(line, sizeof(line), file) == NULL)
	{
		fclose(file);
		return -1;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (strlen(line) > 0)
		{
			count++;
		}
	}

	fclose(file);
	return count;
}

/* get udp count */
int get_udp_connections()
{
	const char *const UDP = "/proc/net/udp";
	FILE *file;
	char line[1024];
	int count = 0;

	file = fopen(UDP, "r");
	if (!file)
	{
		perror("fopen");
		return -1;
	}

	/* ignore title row */
	if (fgets(line, sizeof(line), file) == NULL)
	{
		fclose(file);
		return -1;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (strlen(line) > 0)
		{
			count++;
		}
	}

	fclose(file);
	return count;
}

void get_all_device_io_stats()
{
	const char *const PROC_DISKSTATS = "/proc/diskstats";
	FILE *file;
	char line[1024];
	BlockStats stats;

	file = fopen(PROC_DISKSTATS, "r");
	if (!file)
	{
		perror("fopen");
		return;
	}

	printf("Device I/O Statistics:\n");
	while (fgets(line, sizeof(line), file))
	{
		if (sscanf(line, " %*d %*d %127s %*d %*d %lu %*d %*d %*d %lu",
							 stats.device_name, &stats.read_blocks, &stats.written_blocks) == 3)
		{
			printf("%s - Blocks Read: %lu, Blocks Written: %lu\n",
						 stats.device_name, stats.read_blocks, stats.written_blocks);
		}
	}

	fclose(file);
}

int main(int argc, char *argv[])
{
	const char *const PROC_STAT = "/proc/stat";
	char line[1024];
	FILE *file;

	UNUSED(argc);
	UNUSED(argv);

	if (SIG_ERR == signal(SIGINT, _signal_))
	{
		TRACE("signal()");
		return -1;
	}
	while (!done)
	{
		if (!(file = fopen(PROC_STAT, "r")))
		{
			TRACE("fopen()");
			return -1;
		}
		printf("\033[2J\033[H");
		if (fgets(line, sizeof(line), file))
		{
			printf("CPU Usage: %5.1f%%\n", cpu_util(line));
			fflush(stdout);
		}
		printf("Memory Usage: %.2f%%\n", memory_util());

		printf("Packets Sent: %lu\n", get_network_packets("ens4").sent_packets);
		printf("Packets Received: %lu\n", get_network_packets("ens4").received_packets);

		printf("Active TCP Connections: %d\n", get_tcp_connections());
		printf("Active UDP Connections: %d\n", get_udp_connections());

		get_all_device_io_stats();

		us_sleep(500000);
		fclose(file);
	}
	printf("\rDone!   \n");
	return 0;
}