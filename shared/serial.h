#ifndef SERIAL_HEADERFILE_INCLUDED
#define SERIAL_HEADERFILE_INCLUDED

typedef struct cell {
    int x;
    int y;
    char *symbol;
    char *color;
    char *user;
} Cell;

typedef struct game {
    int rows;
    int cols;

    Cell *bombCells;
    int numBombs;

    Cell *playerCells;
    int numPlayers;
} Game;

char* serializeGame(Game *game);

Game* deserializeGame(char *string);

#endif
