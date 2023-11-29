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

struct cpuStatus
{
	unsigned user;
	unsigned nice;
	unsigned system;
	unsigned idle;
	unsigned iowait;
	unsigned irq;
	unsigned softirq;
	double util;
};

struct systemStatus
{
	unsigned long ctxt;
	long btime;
	unsigned processes;
	unsigned procs_running;
	unsigned procs_blocked;
};

static volatile int done;

static void
_signal_(int signum)
{
	assert(SIGINT == signum);

	done = 1;
}

struct cpuStatus cpu_util(const char *s)
{
	static unsigned sum_, vector_[7];
	unsigned sum, vector[7];
	const char *p;
	uint64_t i;
	struct cpuStatus status;

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
		memset(&status, 0, sizeof(status));
		return status;
	}
	sum = 0.0;
	for (i = 0; i < ARRAY_SIZE(vector); ++i)
	{
		sum += vector[i];
	}
	status.util = (1.0 - (vector[3] - vector_[3]) / (double)(sum - sum_)) * 100.0;
	status.user = vector[0];
	status.nice = vector[1];
	status.system = vector[2];
	status.idle = vector[3];
	status.iowait = vector[4];
	status.irq = vector[5];
	status.softirq = vector[6];
	sum_ = sum;
	for (i = 0; i < ARRAY_SIZE(vector); ++i)
	{
		vector_[i] = vector[i];
	}
	return status;
}

struct systemStatus get_system_status(FILE *file)
{
	struct systemStatus status;
	char line[1024];

	while (fgets(line, sizeof(line), file) != NULL)
	{
		if (sscanf(line, "ctxt %lu", &status.ctxt) == 1)
		{
			continue;
		}
		if (sscanf(line, "btime %ld", &status.btime) == 1)
		{
			continue;
		}
		if (sscanf(line, "processes %u", &status.processes) == 1)
		{
			continue;
		}
		if (sscanf(line, "procs_running %u", &status.procs_running) == 1)
		{
			continue;
		}
		if (sscanf(line, "procs_blocked %u", &status.procs_blocked) == 1)
		{
			continue;
		}
	}

	return status;
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
		struct cpuStatus cpu_status;
		struct systemStatus sys_status = {0};

		if (!(file = fopen(PROC_STAT, "r")))
		{
			TRACE("fopen()");
			return -1;
		}
		while (fgets(line, sizeof(line), file))
		{
			if (strncmp(line, "cpu ", 4) == 0)
			{
				cpu_status = cpu_util(line);
			}
			else
			{
				sys_status = get_system_status(file);
			}
		}
		printf("\033[2J\033[H");
		printf("CPU: %5.1f%%\n", cpu_status.util);
		printf("User: %u\n", cpu_status.user);
		printf("Nice: %u\n", cpu_status.nice);
		printf("System: %u\n", cpu_status.system);
		printf("Idle: %u\n", cpu_status.idle);
		printf("IOwait: %u\n", cpu_status.iowait);
		printf("IRQ: %u\n", cpu_status.irq);
		printf("SoftIRQ: %u\n", cpu_status.softirq);
		printf("ctxt: %lu\n", sys_status.ctxt);
		printf("btime: %ld\n", sys_status.btime);
		printf("processes: %u\n", sys_status.processes);
		printf("procs_running: %u\n", sys_status.procs_running);
		printf("procs_blocked: %u\n", sys_status.procs_blocked);

		fflush(stdout);
		us_sleep(500000);
		fclose(file);
	}
	printf("\rDone!   \n");
	return 0;
}
