#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../../include/protocol.h"

void sendNewGame(int clientSockets[], int numClients, Game *game)
{
    char *message = serializeGame(game);

    int j;
    for( j = 0 ; j < numClients; j++ ) {
        if( send(clientSockets[j] , message , strlen(message) , 0) < 0)
        {
            printf("Send failed\n");
        }
    }
  //  printf(" serialized message %s\n\n\n", message);

    free(message);
}
