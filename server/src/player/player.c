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

pthread_mutex_t authenticationMutex = PTHREAD_MUTEX_INITIALIZER;
void *playerThreadFunc(void *vargp)
{
    int clientSocket = *((int *)vargp);

    addClientSocket(clientSocket);
    char client_message[2000];

    int clientConnected = 1;
    while(clientConnected)
    {
        if(recv(clientSocket, client_message, 2000, 0) <= 0)
        {
           warn("Connection closed. The client cut off!");
           clientConnected = 0;
        }
        else
        {
            AuthenticationRequest *authenticationRequest = NULL;
            MovePlayerRequest *movePlayerRequest = NULL;

            //The client sent a register request
            if((authenticationRequest = deserializeRegisterRequest(client_message)) != NULL)
            {
                //Do the registration
                char str[2100];
                sprintf(str, "RegisterAction: %s", client_message);
                info(str);

                sendRegisterResponse(clientSocket, 0, "OK");

                Game *game = getCurrentGame();
                if(game != NULL)
                {
                    Cell *player = (Cell*) malloc(sizeof(Cell));
                    player->user = authenticationRequest->username;
                    player->x = 0;
                    player->y = 12;
                    player->symbol = "G";
                    player->color = "[0;32m";

                    addPlayer(game, player);
                    sendNewGame(clientSocket, game);
                }

            }
            //The client sent a login request
            else if((authenticationRequest = deserializeLoginRequest(client_message)) != NULL)
            {
                char str[2100];
                sprintf(str, "LoginAction: %s", client_message);
                info(str);

                sendLoginResponse(clientSocket, 0, "OK");
            }
            //The client sent a move request
            else if((movePlayerRequest = deserializeMovePlayerRequest(client_message)) != NULL)
            {
                char str[2100];
                sprintf(str, "MovePlayerAction: %s", client_message);
                info(str);

                Game *game = getCurrentGame();
                int status = movePlayer(game, movePlayerRequest->player, movePlayerRequest->direction);
                int index = indexOfPlayer(game, movePlayerRequest->player);

                sendMovePlayerResponse(clientSocket, &(game->playerCells[index]), status);
            }
        }
    }

    removeClientSocket(clientSocket);
    close(clientSocket);
    pthread_exit(NULL);
}
