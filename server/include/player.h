#ifndef PLAYER_HEADERFILE_INCLUDED
#define PLAYER_HEADERFILE_INCLUDED

#include "serial.h"

void addClientSocket(int clientSocket);

void removeClientSocket(int clientSocket);

void loadRegisteredPlayers(char *filepath);

void broadcastNewGame(Game *game);

void *playerThreadFunc(void *vargp);

#endif
