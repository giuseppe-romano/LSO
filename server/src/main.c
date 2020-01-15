#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

#include "../include/draw.h"
#include "../include/game.h"
#include "../include/protocol.h"

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/* S E R V E R */

void * socketThread(void *arg)
{
    char client_message[2000];
    char buffer[1024];

    int newSocket = *((int *)arg);
    while(1)
    {


        recv(newSocket, client_message, 2000, 0);
        printf("Data received: %s\n", client_message);

        // Send message to the client socket
        //  pthread_mutex_lock(&lock);
        char *message = malloc(sizeof(client_message)+20);
        strcpy(message,"Hello Client : ");
        strcat(message,client_message);
        strcat(message,"\n");
        strcpy(buffer,message);
        free(message);
        pthread_mutex_unlock(&lock);
        sleep(1);
        //send(newSocket,buffer,13,0);

        Game *game = generateNewGame();
        int err;
        int j;
        for(j = 0; j < 40; j++)
        {
            Cell tmp;
            tmp.x = game->cols - 1;
            tmp.y = j;
            tmp.symbol = 'G';
            tmp.color = "\033[1;32m";
            if((err = addPlayer(game, tmp)) != 0)
            {
                printf("Location busy %d \n", err);
            }
        }
        int clients[1] = {newSocket};
        sendNewGame(clients, game);
    }
    printf("Exit socketThread \n");
    close(newSocket);
    pthread_exit(NULL);
}

int main()
{
    Game *game = generateNewGame();
    char* s = serializeGame(game);
    deserializeGame(s);


    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    //Create the socket.
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    // Configure settings of the server address struct
    // Address family = Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(7799);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind the address struct to the socket
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //Listen on the socket, with 40 max connection requests queued
    if(listen(serverSocket,50)==0)
        printf("Listening\n");
    else
        printf("Error\n");

    char buffer[1024];
    char message[1000];
    pthread_t tid[60];
    int i = 0;
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0 )
            printf("Failed to create thread\n");
        if( i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
    }

    // After chatting close the socket
    close(serverSocket);



    /*   int numPlayers = 1;
       Cell playerCells[numPlayers];

       int j;
       for(j = 0; j < numPlayers; j++) {
           Cell tmp;
           tmp.x = j + 1;
           tmp.y = j + 1;
           tmp.symbol = 'G';
           tmp.color = "\033[1;32m";

           playerCells[j] = tmp;
       }
    */
    /*  Game *game = generateNewGame();


      int err;
      int j;
      for(j = 0; j < 40; j++) {
          Cell tmp;
          tmp.x = game->cols - 1;
          tmp.y = j;
          tmp.symbol = 'G';
          tmp.color = "\033[1;32m";
          if((err = addPlayer(game, tmp)) != 0) {
              printf("Location busy %d \n", err);
          }
    sleep(1);
          drawScreen(game);
          sleep(1);
      }*/
    /*
        for(j = 0; j < game->cols; j++) {
            Cell tmp;
            tmp.x = j;
            tmp.y = 5;
            tmp.symbol = 'G';
            tmp.color = "\033[1;32m";



            err = movePlayer(game, tmp, MOVE_RIGHT);
            if(err == ERR_PLAYER_HIT_BOMB) {
                printf("Player hit bomb %d \n", err);
            }
            sleep(1);
            drawScreen(game);
        }
    */
    /*   Cell tmp;
           tmp.x = game->cols - 1;
           tmp.y = 5;
           tmp.symbol = 'G';
           tmp.color = "\033[1;32m";



           err = movePlayer(game, tmp, MOVE_RIGHT);
           if(err == ERR_POSITION_OUT_OF_BOUND) {
               printf("Player ERR_POSITION_OUT_OF_BOUND %d \n", err);
           }
           sleep(3);
           drawScreen(game);
    */
    return 0;
}
