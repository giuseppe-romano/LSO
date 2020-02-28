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

typedef struct authenticationRequest {
    char *username;
    char *password;
} AuthenticationRequest;

typedef struct authenticationResponse {
    int status;
    char *message;
} AuthenticationResponse;

typedef struct movePlayerRequest {
    Cell *player;
    int direction;
} MovePlayerRequest;

typedef struct movePlayerResponse {
    Cell *player;
    int status;
} MovePlayerResponse;

char* serializeGame(Game *game);
Game* deserializeGame(char *string);

char* serializeMovePlayerRequest(Cell *player, char *direction);
MovePlayerRequest* deserializeMovePlayerRequest(char *string);

char* serializeLoginRequest(char *username, char *password);
AuthenticationRequest* deserializeLoginRequest(char *string);

char* serializeRegisterRequest(char *username, char *password);
AuthenticationRequest* deserializeRegisterRequest(char *string);

char* serializeRegisterResponse(int status, char *message);
AuthenticationResponse* deserializeRegisterResponse(char *string);

char* serializeLoginResponse(int status, char *message);
AuthenticationResponse* deserializeLoginResponse(char *string);

char* serializeMovePlayerResponse(Cell *player, int status);
MovePlayerResponse* deserializeMovePlayerResponse(char *string);
#endif
