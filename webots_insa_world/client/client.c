/*
 * Description:  A simple client program to connect to the TCP/IP server thanks to Darren Smith
 */

/*
 * Linux:   compile with gcc -Wall client.c -o client
 */

 #include <stdio.h>
 #include <string.h>
 
 #include <arpa/inet.h>
 #include <netdb.h>
 #include <netinet/in.h>
 #include <sys/socket.h>
 #include <sys/types.h>
 #include <unistd.h>
 
 #include <time.h>
 #include <assert.h>
 #include <errno.h>
#include <bits/pthreadtypes.h>

#include <stdio.h>
#include <stdlib.h>


#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
 
 #define SOCKET_PORT 10020
 #define SOCKET_SERVER "127.0.0.1" /* local host */

typedef struct{
    char command_type;
    int para1; 
    int para2; // buffer size 
} command;
 
 int fd; 

 pthread_mutex_t movement_mutex ;
 
 void *control_robot(void * arg) {
    command *cmd = (command *) arg;
    char buffer[256];

    int period_ms = 1000;
    struct timespec next_activation;
    clock_gettime(CLOCK_MONOTONIC, &next_activation);

    while(1){

        pthread_mutex_lock(&movement_mutex);

        if (cmd->command_type == 'M'){
            printf("MOVINGGGGGG \n");
            sprintf(buffer,"M,%d,%d\n", cmd->para1, cmd->para2);
            send(fd, buffer, strlen(buffer),0);
        }
        else if (cmd->command_type == 'T')
            printf("TURNINGGGGGG \n");
            sprintf(buffer,"T,%d,%d\n", cmd->para1, cmd->para2);
            send(fd, buffer, strlen(buffer),0);
        }

        pthread_mutex_unlock(&movement_mutex);

        // NANO-SLEEP

        next_activation.tv_sec += (period_ms / 1000);
        while (next_activation.tv_nsec >= 1e9)
        {
            next_activation.tv_sec++;
            next_activation.tv_nsec -= 1e9;
        }


        int err;
        do
        {
            // absolute sleep until next_activation
            err = clock_nanosleep(CLOCK_MONOTONIC,
                                  TIMER_ABSTIME,
                                  &next_activation,
                                  NULL);
            // if err is nonzero , we might have woken up
            // too early
        } while (err != 0 && errno == EINTR);
        assert(err == 0);

    }


 
 int main(int argc, char *argv[]) {
   struct sockaddr_in address;
   const struct hostent *server;
   int rc;
   char buffer[256];
 
   /* create the socket */
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if (fd == -1) {
     printf("cannot create socket\n");
     return -1;
   }
 
   /* fill in the socket address */
   memset(&address, 0, sizeof(struct sockaddr_in));
   address.sin_family = AF_INET;
   address.sin_port = htons(SOCKET_PORT);
   server = gethostbyname(SOCKET_SERVER);
 
   if (server)
     memcpy((char *)&address.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
   else {
     printf("cannot resolve server name: %s\n", SOCKET_SERVER);
     close(fd);
     return -1;
   }
 
   /* connect to the server */
   rc = connect(fd, (struct sockaddr *)&address, sizeof(struct sockaddr));
   if (rc == -1) {
     printf("cannot connect to the server\n");
     close(fd);
     return -1;
   }
 
   fflush(stdout);
 
   //send(fd,"M,99,99\n",strlen("M,99,99\n"),0);
   //send(fd,"S,99,99\n",strlen("T,99,99\n"),0);

   printf(" TESTTTTTTTT \n");

   pthread_t tid_move_robot, tid_turn_robot;
   command cmd1 = {'M', 20, 50};
   command cmd2 = {'T', 50, 20};


   if (pthread_create(&tid_move_robot, NULL, control_robot, &cmd1) != 0)
    {
        printf("Error thread move robot\n");
        exit(1);
    }

    if (pthread_create(&tid_turn_robot, NULL, control_robot, &cmd2) != 0)
    {
        printf("Error thread turn robot\n");
        exit(1);
    }

    pthread_join(tid_turn_robot, NULL);
    pthread_join(tid_move_robot, NULL);
    
 
   for (;;) {
     int n = recv(fd, buffer, 256, 0);
     buffer[n] = '\0';
     printf("Received: %s", buffer);
   }
 
   close(fd);


 
   return 0;
 }
 