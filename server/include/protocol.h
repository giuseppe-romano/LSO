#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

#include "game.h"

void sendNewGame(int clientSocket, Game *game);

void sendPlayerHitBomb(int clientSocket, char *userHitBomb);

#endif
