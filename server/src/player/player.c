#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include "serial.h"
#include "logging.h"
#include "../../include/game.h"
#include "../../include/protocol.h"

int clientSockets[100];
int numSockets = 0;

void addClientSocket(int clientSocket)
{
    clientSockets[numSockets++] = clientSocket;
}

void removeClientSocket(int clientSocket)
{
    int j;
    int counter = 0;
    int n = numSockets;
    for( j = 0 ; j < n; j++ ) {
        if(clientSockets[j] != clientSocket) {
            clientSockets[counter++] = clientSockets[j];
        }
        else {
            numSockets--;
        }
    }
}

void notifyNewGame(Game *game)
{
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendNewGame(clientSockets[j], game);
    }
}

void *playerThreadFunc(void *vargp)
{
    int clientSocket = *((int *)vargp);

    addClientSocket(clientSocket);
    char client_message[2000];
 //   char buffer[1024];
/*
    Game *game = getCurrentGame();
    int j;
    int err;
    for(j = 0; j < 2; j++)
    {
        Cell tmp;
        tmp.x = 1;
        tmp.y = j;
        tmp.symbol = "A";
        tmp.color = "\033[1;32m";
        tmp.user = "romagiu";
        if((err = addPlayer(game, tmp)) != 0)
        {
            printf("addPlayer error %d \n", err);
        }
    }

    int counter = 0;


    recv(clientSocket, client_message, 2000, 0);
    printf("Data received: %s\n", client_message);
*/
    // Send message to the client socket
    //  pthread_mutex_lock(&lock);
 /*   char *message = malloc(sizeof(client_message)+20);
    strcpy(message,"Hello Client : ");
    strcat(message,client_message);
    strcat(message,"\n");
    strcpy(buffer,message);
    free(message);
 //   pthread_mutex_unlock(&lock);
    sleep(1);*/


    while(1)
    {
        if(recv(clientSocket, client_message, 2000, 0) < 0)
        {
           error("Receive failed");
        }
        char str[2100];
        sprintf(str, "Data received from client: %s", client_message);
        info(str);

        //send(newSocket,buffer,13,0);
/*
        Cell cell = game->playerCells[counter];
        if((err = movePlayer(game, cell, MOVE_RIGHT)) != 0)
        {
            printf("movePlayer error %d \n", err);
        }

        counter++;
        if(counter >= 5) {
            counter = 0;
        }

        sendNewGame(clientSocket, game);
        sleep(2);
        */
    }

    removeClientSocket(clientSocket);
    close(clientSocket);
    pthread_exit(NULL);
}
