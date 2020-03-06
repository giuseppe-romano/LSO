#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

#include "game.h"

void sendGame(int clientSocket, Game *game);

void sendAddedCell(int clientSocket, Cell *player);
void sendRemovedCell(int clientSocket, Cell *player);

void sendMovePlayerResponse(int clientSocket, Cell *player, int status);

void sendRegisterResponse(int clientSocket, int status, char *message);

void sendLoginResponse(int clientSocket, int status, char *message);

#endif
