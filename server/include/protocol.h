#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

#include "game.h"

void sendNewGame(int clientSockets[], int numClients, Game *game);

int sendPlayerHitBomb(int socket, Cell *player);

#endif
