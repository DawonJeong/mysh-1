#define _POSIX_SOURCE
#define SIGNAL_H_

//#define SIGINT 2
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <signal.h>
#include <stdlib.h>


#include "signal_handlers.h"


void catch_sigint(int signalNo)
{
//	printf("input Ctrl+C?\n");
	signal(SIGINT, SIG_IGN);
}

void catch_sigtstp(int signalNo)
{
	signal(SIGTSTP, SIG_IGN);
}
