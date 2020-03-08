#ifndef GAME_HEADERFILE_INCLUDED
#define GAME_HEADERFILE_INCLUDED

#include "draw.h"

Game* generateNewGame();

Game* getCurrentGame();

Cell* getPlayerByUsername(Game *game, char *username);

int addPlayer(Game* game, Cell *player);

int removePlayer(Game* game, Cell *player);

int movePlayer(Game* game, Cell *player, int direction);

#endif
