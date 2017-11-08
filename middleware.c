
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/signal.h>
#include <fcntl.h>
#include "semaphore.h"

#define SERVER_PROC 111
#define CLIENT_PROC 222
#define MAX_PROCS 15

#define SERVER_PORT 4574
#define MIDDLEWARE_PORT 4575
#define CLIENT_PORT 500

#define FILEKEY "/bin/ps"
#define KEY 1300
#define N_SEM 30

int socket_desc;	/* Socket of the middleware */
char vars[30];		/* Variables used in Database */
int n_vars = 0;		/* Number of variables already created */
int sem_id;			/* Array of semaphores ID */

void SIGINT_handler(int);
void server_thread(void *proc_arg);
void client_thread(void *proc_arg);


typedef struct proc_arg {
	int proc_sock;
	int middle_sock;
	int type;
	pid_t proc_pid;
} proc_arg;


int main(int argc , char *argv[])
{

	signal(SIGINT, SIGINT_handler);
	int proc_sock, c, read_size, ret;
	struct sockaddr_in middleware , proc;
	char proc_message[500], *token = NULL;
	pthread_t tID[MAX_PROCS];
	pid_t proc_id;
	key_t key;
	unsigned short *init;
	proc_arg *args;
	int i, n_procs_connected = 0;


	/* For the semaphore creation */
	union semun {
	   int val;
	   struct semid_ds *semstat;
	   unsigned short *array;
	} arg;

	/* For "down" and "up" operations */
	struct sembuf sem_oper;

	key = ftok(FILEKEY, KEY);
	if (key == -1) {
    	perror("Error with the key\n");
		exit(-1); 
	}

	/***************** Creation of an array of semaphores with size N_SEM *****************/
	ret = create_Semaphore(key, N_SEM, &sem_id);
	if(ret == ERROR) {
		perror("Error in the creation of the semaphore\n");
		exit(-1);
	}

	if(ret == 1) {
		perror("The semaphore already exists\n");
		//exit(-1);
	}
	

	/***************** Semaphores' values Initialization *****************/
	init = (unsigned short *) malloc(N_SEM * sizeof(init[0]));
	for (i = 0; i < N_SEM; i++) {
		init[i] = 1;
	}

	/***************** Array of semaphore initialization *****************/
	//fprintf(stdout, "\nInitialization of mutex's array values\n\n");
	if (initialize_Semaphore(sem_id, init) == ERROR) {
		perror("Error in the inizialitation of the array of semaphores\n");
		/* We erase the array of mutexes */
		if (erase_Semaphore(sem_id) == ERROR) {
			perror("Error erasing semaphores\n");
		}
		free(init);
		exit(-1);
	}

	free(init);

    //Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
		return -1;
	}
	puts("Socket created");

    //Prepare the sockaddr_in structure
	middleware.sin_family = AF_INET;
	middleware.sin_addr.s_addr = htonl(INADDR_ANY);
	middleware.sin_port = htons(MIDDLEWARE_PORT);

    //Bind
	if(bind(socket_desc,(struct sockaddr *)&middleware , sizeof(middleware)) < 0)
	{
        //print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("Bind done");

    //Listen
	listen(socket_desc, 10);

    //Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	while(1) {
		args = (proc_arg *) malloc(sizeof(proc_arg));

	    //accept connection from an incoming proccess
		proc_sock = accept(socket_desc, (struct sockaddr *)&proc, (socklen_t*)&c);
		if (proc_sock < 0) {
			perror("Accept failed");
			return 1;
		}

		puts("Connection accepted");

		read_size = recv(proc_sock, proc_message, 500, 0);
		proc_message[read_size] = '\0';

		switch (read_size) {
			case 0:
				puts("Proccess disconnected");
				fflush(stdout);
				break;
			case -1:
				perror("Recv failed");
				break; 
		}

		n_procs_connected++;

		token = strtok(proc_message, " ");


		/* Thread Arguuments */
		args->proc_sock = proc_sock;
		fflush(stdout);
		args->middle_sock = socket_desc;
		proc_id = atol(strtok(NULL, " "));
		args->proc_pid = (pid_t) proc_id;


		/* Assign a thread depending on the kind of proccess */
		if (!strcmp(token, "CLIENT")) {
			args->type = CLIENT_PROC;
			pthread_create(&tID[n_procs_connected], NULL, (void *) client_thread, args);
		} else if (!strcmp(token, "SERVER")) {
			args->type = SERVER_PROC;
			pthread_create(&tID[n_procs_connected], NULL, (void *) server_thread, args);
		} else {
			puts("Error identifying the type of proccess");
			free(args);
			return 0;
		}

		// free(args);
	}

	for (i = 0; i < n_procs_connected; i++) {
		pthread_join(tID[i], NULL);
	}

	return 0;
}



void SIGINT_handler(int sig) {
	fflush(stdout);
    close(socket_desc);
    erase_Semaphore(sem_id);
    exit(1);
}

void server_thread(void *args) {
	int read_size;
	char server_msg[500];
	proc_arg *proc_data = (proc_arg *) args;

	printf("New server connected with pid %ld\n", (long) proc_data->proc_pid);


	while(1) {
		read_size = recv(proc_data->proc_sock, server_msg, 500, 0);
		server_msg[read_size] = '\0';

		switch (read_size) {
			case 0:
				puts("Proccess disconnected");
				fflush(stdout);
				pthread_exit(NULL);
				break;
			case -1:
				perror("Recv failed");
				pthread_exit(NULL);
				break; 
		}

		printf("Server_msg: %s\n\n", server_msg);
		fflush(stdout);
	}

	pthread_exit(NULL);
}

void client_thread(void *args) {
	int read_size;
	char client_msg[500];
	proc_arg *proc_data = (proc_arg *) args;

	printf("New client connected with pid %ld\n", (long) proc_data->proc_pid);
	
	while(1) {
		read_size = recv(proc_data->proc_sock, client_msg, 500, 0);
		client_msg[read_size] = '\0';

		switch (read_size) {
			case 0:
				puts("Proccess disconnected");
				fflush(stdout);
				pthread_exit(NULL);
				break;
			case -1:
				printf("PROC_SOCK: %d", proc_data->proc_sock);
				fflush(stdout);
				perror("Recv failed");
				pthread_exit(NULL);
				break; 
		}

		/* TODO Receive client transactions and manipulate semaphores */

		printf("Client_msg: %s\n\n", client_msg);
		fflush(stdout);
	}

	pthread_exit(NULL);
}