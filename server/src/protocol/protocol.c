#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serial.h"

#include "../../include/game.h"
#include "../../include/protocol.h"

void sendNewGame(int clientSocket, Game *game)
{
    char *message = serializeGame(game);
    write(clientSocket, message, strlen(message));
    free(message);
}

void sendPlayerHitBomb(int clientSocket, char *userHitBomb)
{
    write(clientSocket, userHitBomb, strlen(userHitBomb));
}
