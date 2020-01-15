#ifndef GAME_HEADERFILE_INCLUDED
#define GAME_HEADERFILE_INCLUDED

#define ERR_CELL_BUSY 1
#define ERR_PLAYER_NOT_FOUND 2
#define ERR_INVALID_DIRECTION 3
#define ERR_PLAYER_HIT_BOMB 4
#define ERR_POSITION_OUT_OF_BOUND 5
#define ERR_MAX_NUMBER_OF_PLAYER_REACHED 6

#define MOVE_UP 10
#define MOVE_RIGHT 11
#define MOVE_DOWN 12
#define MOVE_LEFT 13

typedef struct cell {
    int x;
    int y;
    char symbol;
    char *color;
} Cell;

typedef struct game {
    int rows;
    int cols;

    Cell *bombCells;
    int numBombs;

    Cell *playerCells;
    int numPlayers;
} Game;

Game* generateNewGame();

int addPlayer(Game* game, Cell player);

int removePlayer(Game* game, Cell player);

int movePlayer(Game* game, Cell player, int direction);

#endif
