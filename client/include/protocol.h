#ifndef PROTOCOL_HEADERFILE_INCLUDED
#define PROTOCOL_HEADERFILE_INCLUDED
#include "serial.h"

void setServerSocket(int _socket);

void sendRegisterRequest(char *username, char *password, char *color, char *symbol);
void sendLoginRequest(char *username, char *password);
void sendLogoutRequest(char *username);
void sendMovePlayerRequest(Cell *player, int direction);

#endif
