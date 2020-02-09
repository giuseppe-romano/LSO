#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include "../../include/game.h"
#include "../../include/protocol.h"

void *playerThreadFunc(void *vargp)
{
    char client_message[2000];
 //   char buffer[1024];

    Game *game = generateNewGame();
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

    int newSocket = *((int *)vargp);
    int clients[1] = {newSocket};

    recv(newSocket, client_message, 2000, 0);
    printf("Data received: %s\n", client_message);

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

        //send(newSocket,buffer,13,0);

        Cell cell = game->playerCells[counter];
        if((err = movePlayer(game, cell, MOVE_RIGHT)) != 0)
        {
            printf("movePlayer error %d \n", err);
        }

        counter++;
        if(counter >= 2) {
            counter = 0;
        }

/*
        for(j = 0; j < game->numPlayers; j++)
        {
            Cell cell = game->playerCells[j];
            if((err = movePlayer(game, cell, MOVE_RIGHT)) != 0)
            {
                printf("movePlayer error %d \n", err);
            }

            sendNewGame(clients, game);
            sleep(1);
        }
*/

        sleep(2);
        sendNewGame(clients, 1, game);
            drawScreen(game);

        sleep(1);
    }
    printf("Exit socketThread \n");
    close(newSocket);
    pthread_exit(NULL);
}
