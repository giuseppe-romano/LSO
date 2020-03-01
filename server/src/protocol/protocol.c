#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serial.h"

#include "../../include/game.h"
#include "../../include/protocol.h"

void sendGame(int clientSocket, Game *game)
{
    char *message = serializeGame(game);
    write(clientSocket, message, strlen(message));
    free(message);
}

void sendMovePlayerResponse(int clientSocket, Cell *player, int status)
{
    char *response = serializeMovePlayerResponse(player, status);
    write(clientSocket, response, strlen(response));
    free(response);
}

void sendRegisterResponse(int clientSocket, int status, char *message)
{
    char *response = serializeRegisterResponse(status, message);
    write(clientSocket, response, strlen(response));
    free(response);
}

void sendLoginResponse(int clientSocket, int status, char *message)
{
    char *response = serializeLoginResponse(status, message);
    write(clientSocket, response, strlen(response));
    free(response);
}
