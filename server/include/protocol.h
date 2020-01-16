#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

#include "game.h"

void sendNewGame(int clientSockets[], int numClients, Game *game);

void sendPlayerHitBomb(int clientSockets[], int numClients, char *userHitBomb);

#endif
