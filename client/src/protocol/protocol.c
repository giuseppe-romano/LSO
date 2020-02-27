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

void sendRegisterAction(char *username, char *password)
{
    char *message = serializeRegisterAction(username, password);
    write(serverSocket, message, strlen(message));
    free(message);
}

void sendLoginAction(char *username, char *password)
{
    char *message = serializeLoginAction(username, password);
    write(serverSocket, message, strlen(message));
    free(message);
}
