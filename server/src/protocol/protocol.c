#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serial.h"
#include "logging.h"

#include "../../include/game.h"
#include "../../include/protocol.h"

void logMessage(char *message)
{
    char logging[2000];
    sprintf(logging, "Sending message '%s' over the network...", message);
    infoProtocol(logging);
}

void sendGame(int clientSocket, Game *game)
{
    char *message = serializeGame(game);
    logMessage(message);
    write(clientSocket, message, strlen(message));
    infoProtocol("Message sent!");
    free(message);
}

void sendAddedCell(int clientSocket, Cell *player)
{
    char *message = serializeAddedCell(player);
    logMessage(message);
    write(clientSocket, message, strlen(message));
    infoProtocol("Message sent!");
    free(message);
}

void sendRemovedCell(int clientSocket, Cell *player)
{
    char *message = serializeRemovedCell(player);
    logMessage(message);
    write(clientSocket, message, strlen(message));
    infoProtocol("Message sent!");
    free(message);
}

void sendMovePlayerResponse(int clientSocket, Cell *player, int status)
{
    char *response = serializeMovePlayerResponse(player, status);
    logMessage(response);
    write(clientSocket, response, strlen(response));
    infoProtocol("Message sent!");
    free(response);
}

void sendRegisterResponse(int clientSocket, int status, char *message)
{
    char *response = serializeRegisterResponse(status, message);
    logMessage(response);
    write(clientSocket, response, strlen(response));
    infoProtocol("Message sent!");
    free(response);
}

void sendLoginResponse(int clientSocket, int status, char *message)
{
    char *response = serializeLoginResponse(status, message);
    logMessage(response);
    write(clientSocket, response, strlen(response));
    infoProtocol("Message sent!");
    free(response);
}
