#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <sys/stat.h>

#include "commands.h"
#include "built_in.h"
#include "signal_handlers.h"

#define SOCK_PATH "tpf_unix_sock.server"
#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"
//#define DATA "Hello from client"


void *clientsocket(void*a);

void *serversocket(void*b);
int fd;
int fd2;
int *num;
struct single_command* com2; 
struct single_command* com;
struct single_command* com3;


static struct built_in_command built_in_commands[] = {
	{"cd", do_cd, validate_cd_argv},
	{"pwd", do_pwd, validate_pwd_argv},
	{"fg",do_fg,validate_fg_argv}
};

static int is_built_in_command(const char* command_name)
{
	static const int n_built_in_commands = sizeof(built_in_commands)/sizeof(built_in_commands[0]);

	for(int i=0; i< n_built_in_commands; ++i){
		if(strcmp(command_name, built_in_commands[i].command_name)==0) {
			return i;
		}
	}

	return -1; //Not found

}


int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
	//t ifg=0;
	
	signal(SIGINT, catch_sigint);
	signal(SIGTSTP, catch_sigtstp);
	
	char *argvls[] = {"ls",NULL};
	char *argvcat[] = {"cat","/etc/hosts",NULL};
	char *argvvim[] = {"/usr/bin/vim",NULL};
	char *env[] = { "PATH=/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin",NULL};
	
	if(n_commands > 0){
	//struct single_command* com = (*commands);
		com = (*commands);
		com3 = com;
		assert(com->argc != 0);
		
		int built_in_pos = is_built_in_command(com->argv[0]);

		if(built_in_pos != -1){
				if(built_in_commands[built_in_pos].command_validate(com->argc, com->argv)){				
				if(built_in_commands[built_in_pos].command_do(com->argc,com->argv) != 0){
				//	 if(strcmp(com->argv[0],"fg")==0){
					//   	printf("\t");
					 //  	for(int i=0; i <com3->argc-1 ; i++){
					 //  		 printf("%s",com3->argv[i]);
					//	}
				//	 }else{
							fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
				//	}
				}
		} else {
			fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
			return -1;
			}
		}else if(strcmp(com->argv[0],"exit")==0){
			return 1;
		}else if(strcmp(com->argv[0],"")==0){
			return 0;
		}else if(built_in_pos == -1){
		//	pid_t pid = fork();
		//	int pid = fork();

			if(n_commands >= 2){
				com2 = (*commands)+1;
				char *gargv[] = {"/bin/grep","localhost",NULL};
				char *caargv[] = {"/bin/cat","/etc/hosts",NULL};
				char ab[256];
				pthread_t threads[2];
				
				pthread_create(&threads[0],NULL,&serversocket,NULL);
			///	pthread_create(&threads[1],NULL,&clientsocket,NULL);
				pthread_join(threads[0],(void**)&ab);
			//	int a = pthread_join(threads[1],(void**)&ab);
				
					
			}
			
			//background process
			
			if(strcmp(com->argv[com->argc-1],"&")==0){
				int pid1=fork();
				if(pid1==0){
					printf("%d\n",getpid());
					*num = getpid();
					execv(com->argv[0],com->argv);
					exit(1);

				}else{
					//not wait
					}
				}
			
			
			int pid= fork();
			
			if(pid == 0){
				if(strcmp(com->argv[0],"ls")==0)
			 		execve("/bin/ls",argvls,env);
					else if(strcmp(com->argv[0],"cat")==0)
			   		execve("/bin/cat",argvcat,env);
				 else if(strcmp(com->argv[0],"vim")==0){
				  	 execve("/usr/bin/vim",argvvim,env);
				   	 //  wait(NULL);
				}else
					execv(com->argv[0],com->argv);
			}else{
				if(strcmp(com->argv[com->argc-1],"&")!=0)
					wait(NULL);
			}

			memset((*commands),0,8096);
		}else{
			fprintf(stderr, "%s: command not found\n", com->argv[0]);
			return -1;
		}

	}

		return 0;
}

void free_commands(int n_commands, struct single_command(*commands)[512])
{
	for(int i=0; i< n_commands; ++i){
		struct single_command *com = (*commands) + i;
		int argc = com->argc;
		char** argv = com->argv;

		for(int j=0; j<argc; ++j){
			free(argv[j]);
		}

		free(argv);

	}

	memset((*commands), 0, sizeof(struct single_command) * n_commands);

}


void *clientsocket(void*a)
{

	int client_socket, rc, len;
//	int rc,len;
	struct sockaddr_un server_sockaddr;
	struct sockaddr_un client_sockaddr;
	char buf[256];
	memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
	memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

	client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if( client_socket == -1){
		exit(1);
	}

	client_sockaddr.sun_family = AF_UNIX;
	strcpy(client_sockaddr.sun_path, CLIENT_PATH);
	len = sizeof(client_sockaddr);

	unlink(CLIENT_PATH);
	rc = bind(client_socket, (struct sockaddr*) &client_sockaddr,len);
	if(rc == -1){
	printf("CLIENT BIND ERROR");
		close(client_socket);
		exit(1);
	}

	server_sockaddr.sun_family = AF_UNIX;
	strcpy(server_sockaddr.sun_path, SERVER_PATH);
	rc = connect(client_socket, (struct sockaddr*)&server_sockaddr,len);
	if(rc == -1){
		close(client_socket);
		exit(1);
	}
	
	
	strcpy(buf, "/bin/cat /etc/hosts");

//	char* arg[] = {"/bin/cat","/etc/hosts",NULL};

	int pid2 = fork();
	if(pid2==0){
		fd = dup(STDOUT_FILENO);
	//	dup2(fd,STDOUT_FILENO);
	//	fd2=dup(fd);
	//	close(fd);
		execv(com->argv[0],com->argv);
	//	dup2(fd,STDOUT_FILENO);
	//	printf("Still in client\n");
	} else{
		exit(1);
	}
//	rc = send(client_socket, buf, strlen(buf), 0);
//	if(rc == -1){
//		printf("SEND ERROR\n");
//		close(client_socket);
//		exit(1);
//	}
	memset(buf, 0, sizeof(buf));
//	dup2(client_socket,STDOUT_FILENO);
//	execv(arg[0],arg);
//	close(client_socket);

	memset(buf, 0, 256);
 
	close(client_socket);
}


void *serversocket(void*b){
	int pid2= fork();
	if(pid2==0){
	int server_sock, client_sock, len, rc;
	int bytes_rec=0;
   
  struct sockaddr_un server_sockaddr;
  struct sockaddr_un client_sockaddr;

	char buf[256]; 
	int backlog = 10;
 
	memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
	memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf,0,256);
 
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(server_sock == -1){
       exit(1);
    }
	
	server_sockaddr.sun_family = AF_UNIX;
	strcpy(server_sockaddr.sun_path, SOCK_PATH);
	len = sizeof(server_sockaddr);
	 
	unlink(SOCK_PATH);
	rc = bind(server_sock, (struct sockaddr*)&server_sockaddr,len);
	if(rc == -1){
		printf("SERVER BIND ERROR");
		close(server_sock);
	   exit(1);
	}
	 
	rc = listen(server_sock, backlog);
	if( rc == -1){
	printf("LISTEN ERROR");
	    close(server_sock);
	    exit(1);
	}
//	fd2=dup(fd);
	
    pthread_t thread[1];
	char bu[256];
	pthread_create(&thread[0],NULL,&clientsocket,NULL);
	int r = pthread_join(thread[0],(void**)&bu);
 	
	
//	char *gargv[] = {"bin/grep","localhost",NULL};
	

	client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &len);
	 if(client_sock == -1){
		printf("ACCEPT ERROR\n");
		close(server_sock);
		 close(client_sock);
		 exit(1);
	 }
	fd=dup(STDIN_FILENO);
//	dup2(fd,STDIN_FILENO);
//	close(fd);
	execv(com2->argv[0],com2->argv);


 //	bytes_rec= recv(client_sock, buf, sizeof(buf),0);
//	bytes_rec = recv(client_socket,buf,sizeof(buf),0);
/*	if(bytes_rec == -1){
		 close(server_sock);
		 close(client_sock);
		 exit(1);
	}else{
		  printf("Data recieved = %s\n",buf);
	}*/

	memset(buf, 0, 256);

	close(server_sock);
	close(client_sock);
		   //thread_exit(NULL);
 }else{wait(NULL);}

}
