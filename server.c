
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

#define SERVER_PROC 111
#define CLIENT_PROC 222

#define SERVER_PORT 4574
#define MIDDLEWARE_PORT 4575
#define CLIENT_PORT 5001

int sock;

void SIGINT_handler(int);

int main(int argc, char *argv[])
{

    signal(SIGINT, SIGINT_handler);
    struct sockaddr_in middleware;
    char message[500];

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    middleware.sin_addr.s_addr = htonl(INADDR_ANY);
    middleware.sin_family = AF_INET;
    middleware.sin_port = htons(MIDDLEWARE_PORT);

    //Connect to remote middleware
    if (connect(sock, (struct sockaddr *)&middleware, sizeof(middleware)) < 0)
    {
        perror("Connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    sprintf(message, "SERVER %ld", (long) getpid());
    if( send(sock, message, sizeof(message), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }


    /*******************************************************************************************************/


    while(1) {
        printf("\n");
        scanf("%s", message);
        if(send(sock, message, sizeof(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
    }

    /*******************************************************************************************************/

    close(sock);
    return 0;
}

void SIGINT_handler(int sig) {
    close(sock);
    exit(1);
}
