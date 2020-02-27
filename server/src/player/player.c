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
            AuthenticationAction *authenticationAction = NULL;
            MovePlayerAction *movePlayerAction = NULL;
            if((authenticationAction = deserializeRegisterAction(client_message)) != NULL)
            {
                authenticationAction->username
                char str[2100];
                sprintf(str, "RegisterAction: %s", client_message);
                info(str);
            }
            else if((authenticationAction = deserializeLoginAction(client_message)) != NULL)
            {
                char str[2100];
                sprintf(str, "LoginAction: %s", client_message);
                info(str);
            }
            else if((movePlayerAction = deserializeMovePlayerAction(client_message)) != NULL)
            {
                char str[2100];
                sprintf(str, "MovePlayerAction: %s", client_message);
                info(str);
            }
        }
    }

    removeClientSocket(clientSocket);
    close(clientSocket);
    pthread_exit(NULL);
}
