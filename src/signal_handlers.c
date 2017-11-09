#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>


#include "signal_handlers.h"

void catch_sigint(int signalNo)
{
	signal(SIGINT, catch_sigint);
	printf("Can't be terminated using Ctrl+C\n");
	fflush(stdout);
}

void catch_sigtstp(int signalNo)
{

}
