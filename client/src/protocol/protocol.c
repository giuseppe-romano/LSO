#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serial.h"
#include "logging.h"

#include "../../include/protocol.h"

int serverSocket;
void setServerSocket(int _socket)
{
    serverSocket = _socket;
}

void logMessage(char *message)
{
    char logging[2000];
    sprintf(logging, "Sending message '%s' over the network...", message);
    info(logging);
}

void sendRegisterRequest(char *username, char *password, char *color, char *symbol)
{
    char *message = serializeRegisterRequest(username, password, color, symbol);
    logMessage(message);

    write(serverSocket, message, strlen(message));
    info("Message sent!");
    free(message);
}

void sendLoginRequest(char *username, char *password)
{
    char *message = serializeLoginRequest(username, password);
    logMessage(message);

    write(serverSocket, message, strlen(message));
    info("Message sent!");
    free(message);
}

void sendLogoutRequest(char *username)
{
    char *message = serializeLogoutRequest(username);
    logMessage(message);

    write(serverSocket, message, strlen(message));
    info("Message sent!");
    free(message);
}

void sendMovePlayerRequest(Cell *player, int direction)
{
    char *message = serializeMovePlayerRequest(player, direction);
    logMessage(message);

    write(serverSocket, message, strlen(message));
    info("Message sent!");
    free(message);
}
