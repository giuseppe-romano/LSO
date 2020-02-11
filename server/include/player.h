#ifndef PLAYER_HEADERFILE_INCLUDED
#define PLAYER_HEADERFILE_INCLUDED

void addClientSocket(int clientSocket);

void removeClientSocket(int clientSocket);

void notifyNewGame(Game *game);

void *playerThreadFunc(void *vargp);

#endif
