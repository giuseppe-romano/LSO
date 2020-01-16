#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "draw.h"
#include "serial.h"

//#include <pthread.h>
/*
void * clientThread(void *arg)
{
  printf("In thread\n");
  char message[1000];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  // Create the socket.
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  //Configure settings of the server address
 // Address family is Internet
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function
  serverAddr.sin_port = htons(7799);
 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("localhost");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    strcpy(message,"Hello");
   if( send(clientSocket , message , strlen(message) , 0) < 0)
    {
            printf("Send failed\n");
    }
    //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0)
    {
       printf("Receive failed\n");
    }
    //Print the received message
    printf("Data received: %s\n",buffer);
    close(clientSocket);
    pthread_exit(NULL);
}
*/

int main(){
  printf("In thread\n");
  char message[1000];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  // Create the socket.
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  //Configure settings of the server address
 // Address family is Internet
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function
  serverAddr.sin_port = htons(7799);
 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
    //Connect the socket to the server using the address
    addr_size = sizeof(serverAddr);


    while(1)
    {
        connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
        strcpy(message,"Hello Server");

        if( send(clientSocket , message , strlen(message) , 0) < 0)
        {
            printf("Send failed\n");
        }
        memset(buffer, '\0', 1024);
        //Read the message from the server into the buffer
        if(recv(clientSocket, buffer, 1024, 0) < 0)
        {
           printf("Receive failed\n");
        }
        //Print the received message
        printf("Data received: %s\n\n", buffer);

        Game *game = deserializeGame(buffer);
        char *s = serializeGame(game);
        printf("Deserialized game: %s\n\n\n", s);

    //    drawScreen(game);
        sleep(1);
    }
    close(clientSocket);
 //   pthread_exit(NULL);
  return 0;
}
