#ifndef PLAYER_HEADERFILE_INCLUDED
#define PLAYER_HEADERFILE_INCLUDED

#include "serial.h"

void addClientSocket(int clientSocket);

void shutdownClientSockets();

void removeClientSocket(int clientSocket);

void addPlayerThread(pthread_t playerThread);

void killPlayerThreads();

Player* getRegisteredPlayers();

Player* getConnectedPlayers();

void loadRegisteredPlayers(char *filepath);

void broadcastNewGame(Game *game);

void *playerThreadFunc(void *vargp);

#endif
