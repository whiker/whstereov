#include <sys/time.h>
#include "util.h"

void errExit()
{
	printf("error: %s", g_error.c_str());
    exit(0);
}

void markTime(int t)
{
    if (t == 0)
    {
        gettimeofday(&g_timeStart, NULL);
    }
    else
    {
        gettimeofday(&g_timeEnd, NULL);
        if (g_timeEnd.tv_usec < g_timeStart.tv_usec)
        {
            g_timeEnd.tv_sec--;
            g_timeEnd.tv_usec += 1000000;
        }
		int sec = g_timeEnd.tv_sec - g_timeStart.tv_sec;
		int usec = (g_timeEnd.tv_usec - g_timeStart.tv_usec) / 1000;
		printf("time: %d.%03d\n", sec, usec);
    }
}