#define _POSIX_SOURCE
#define SIGNAL_H_
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <bits/sigaction.h>
#include <stdlib.h>


#include "signal_handlers.h"

void catch_sigint(int signalNo)
{
	
}

void catch_sigtstp(int signalNo)
{

}
