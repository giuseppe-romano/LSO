#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED

#include "game.h"

void sendNewGame(int clientSocket, Game *game);

void sendMovePlayerResponse(int clientSocket, Cell *player, int status);

void sendRegisterResponse(int clientSocket, int status, char *message);

void sendLoginResponse(int clientSocket, int status, char *message);

#endif
