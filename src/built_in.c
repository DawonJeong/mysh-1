#define _POSIX_SOURCE
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include <signal.h>

#include "built_in.h"
#include "commands.h"



int do_cd(int argc, char** argv) {
  if (!validate_cd_argv(argc, argv))
    return -1;

	//char path[256]=" ";

//	strcpy(path,argv[1]);

//	if(strcmp(path,"~")==0)
//		chdir(getenv("HOME"));
		
	  if (chdir(argv[1]) == -1)
   		 return -1;

  return 0;
}

int do_pwd(int argc, char** argv) {
  if (!validate_pwd_argv(argc, argv))
    return -1;

  char curdir[PATH_MAX];

  if (getcwd(curdir, PATH_MAX) == NULL)
    return -1;

  printf("%s\n", curdir);

  return 0;
}

int do_fg(int argc, char** argv) {
  if (!validate_fg_argv(argc, argv))
    return -1;

  // TODO: Fill this.
	
	if(setpgid(getpid(),0)!=0)
		perror("setpgid() error");
	else{
		printf("%d running",getpgrp());
		
	}
  return 0;

}

int validate_cd_argv(int argc, char** argv) {
  if (argc != 2) return 0;
  if (strcmp(argv[0], "cd") != 0) return 0;

  struct stat buf;
  stat(argv[1], &buf);

  if (!S_ISDIR(buf.st_mode)) return 0;

  return 1;
}

int validate_pwd_argv(int argc, char** argv) {
  if (argc != 1) return 0;
  if (strcmp(argv[0], "pwd") != 0) return 0;

  return 1;
}

int validate_fg_argv(int argc, char** argv) {
  if (argc != 1) return 0;
  if (strcmp(argv[0], "fg") != 0) return 0;
  
  return 1;
}
