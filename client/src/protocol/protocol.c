#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "serial.h"

#include "../../include/protocol.h"

int serverSocket;
void setServerSocket(int _socket)
{
    serverSocket = _socket;
}

void sendRegisterRequest(char *username, char *password, char *color, char *symbol)
{
    char *message = serializeRegisterRequest(username, password, color, symbol);
    write(serverSocket, message, strlen(message));
    free(message);
}

void sendLoginRequest(char *username, char *password)
{
    char *message = serializeLoginRequest(username, password);
    write(serverSocket, message, strlen(message));
    free(message);
}
