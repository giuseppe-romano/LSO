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

typedef struct movePlayerAction {
    Cell *player;
    char *direction;
} MovePlayerAction;

typedef struct authenticationAction {
    char *username;
    char *password;
} AuthenticationAction;

char* serializeGame(Game *game);
Game* deserializeGame(char *string);

char* serializeMovePlayerAction(Cell *player, char *direction);
MovePlayerAction* deserializeMovePlayerAction(char *string);

char* serializeLoginAction(char *username, char *password);
AuthenticationAction* deserializeLoginAction(char *string);

char* serializeRegisterAction(char *username, char *password);
AuthenticationAction* deserializeRegisterAction(char *string);
#endif
