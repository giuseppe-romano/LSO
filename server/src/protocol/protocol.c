#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../include/protocol.h"

void sendNewGame(int clientSockets[], int numClients, Game *game)
{
    char *message = serializeGame(game);

    printf("Sending game %s\n", message);

    int j;
    for( j = 0 ; j < numClients; j++ ) {
        if( write(clientSockets[j], message, strlen(message)) < 0)
        {
            printf("Send failed\n");
        }
    }

    free(message);
}

void sendPlayerHitBomb(int clientSockets[], int numClients, char *userHitBomb)
{

}
