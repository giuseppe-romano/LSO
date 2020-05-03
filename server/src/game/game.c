#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../../include/game.h"
#include "../../include/player.h"
#include "logging.h"

#define NUM_ROWS 20
#define NUM_COLS 30

Game *currentGame = NULL;

void deployRandomBombs(Game *game) {
    infoGame("Deploying bombs randomly...");

    /* Intializes random number generator */
    time_t t;
    srand((unsigned) time(&t));

    int numBombs = 3 * (game->rows + game->cols);

    Cell *headCell = NULL;
    Cell *currentCell = NULL;

    int j;
    for( j = 0 ; j < numBombs; j++ ) {
        Cell *b = (Cell*) malloc(sizeof(Cell));
        int x = rand() % game->cols;
        b->x = x == 0 ? x + 1 : ((x == game->cols - 1) ? x - 1 : x);
        b->y = rand() % game->rows;
        b->symbol = "X";
        b->color = "red";
        b->next = NULL;

        if(headCell == NULL)
        {
            headCell = b;
            currentCell = headCell;
        }
        else
        {
            currentCell->next = b;
            currentCell = currentCell->next;
        }
    }
    game->bombCells = headCell;

    char logMessage[2000];
    sprintf(logMessage, "Deployed %d bombs", numBombs);
    infoGame(logMessage);

}

Game* generateNewGame() {
    infoGame("Generating new game...");

    Game *game = (Game*) malloc(sizeof(Game));
    game->rows = NUM_ROWS;
    game->cols = NUM_COLS;
    game->playerCells = NULL;
    game->bombCells = NULL;

    Player *players = getConnectedPlayers();

    Player *current = players;
    Cell *headCell = NULL;
    Cell *currentCell = NULL;

    while (current != NULL) {
        if(headCell == NULL)
        {
            headCell = generateRandomPlayerCell(current);
            currentCell = headCell;
        }
        else
        {
            currentCell->next = generateRandomPlayerCell(current);
            currentCell = currentCell->next;
        }
        current = current->next;
    }
    game->playerCells = headCell;

    deployRandomBombs(game);

    if(currentGame != NULL)
    {
        infoGame("Freeing old game...");
        free(currentGame);
    }

    currentGame = game;
    infoGame("New game generated succesfully!");

    return game;
}

Game* getCurrentGame() {
    return currentGame;
}

Cell* getPlayerByUsername(Game *game, char *username) {
    Cell *playerCell = NULL;
    Cell *currentCell = game->playerCells;

    while(currentCell != NULL)
    {
        if(strcmp(currentCell->user, username) == 0) {
            playerCell = currentCell;
        }
        currentCell = currentCell->next;
    }

    return playerCell;
}

int hasBombAt(Game* game, int x, int y) {
    int found = 0;
    Cell *currentBombCell = game->bombCells;

    while(currentBombCell != NULL)
    {
        if(currentBombCell->x == x && currentBombCell->y == y)
        {
            found = 1;
        }
        currentBombCell = currentBombCell->next;
    }

    return found;
}

int hasPlayerAt(Game* game, int x, int y) {
    int found = 0;
    Cell *currentCell = game->playerCells;

    while(currentCell != NULL)
    {
        if(currentCell->x == x && currentCell->y == y)
        {
            found = 1;
        }
        currentCell = currentCell->next;
    }

    return found;
}

int addPlayer(Game* game, Cell *player) {
    char logMessage[2000];
    sprintf(logMessage, "Adding a new player '%s'", player->user);
    infoGame(logMessage);

    if(hasPlayerAt(game, player->x, player->y) == 1)
    {
        return ERR_CELL_BUSY;
    }

    if(game->playerCells == NULL)
    {
        game->playerCells = player;
    }
    else
    {
        Cell *parentCell = game->playerCells;
        Cell *currentCell = game->playerCells;
        while(currentCell != NULL)
        {
            parentCell = currentCell;
            currentCell = currentCell->next;
        }
        parentCell->next = player;

    }
    infoGame("New player added succesfully!");
    return 0;
}

int removePlayer(Game* game, Cell *player) {
    char logMessage[2000];
    sprintf(logMessage, "Removing player '%s'", player->user);
    infoGame(logMessage);

    Cell *p = getPlayerByUsername(game, player->user);
    if(p == NULL) {
        sprintf(logMessage, "Player '%s' not found", player->user);
        warnGame(logMessage);

        return ERR_PLAYER_NOT_FOUND;
    }

    Cell *tmp = NULL;
    if(strcmp(game->playerCells->user, player->user) == 0)
    {
        tmp = game->playerCells;
        game->playerCells = game->playerCells->next;
    }
    else
    {
        Cell *parentCell = game->playerCells;
        Cell *currentCell = game->playerCells;
        while(currentCell != NULL)
        {
            if(strcmp(currentCell->user, player->user) == 0)
            {
                tmp = currentCell;
                parentCell->next = currentCell->next;
            }
            parentCell = currentCell;
            currentCell = currentCell->next;
        }

        free(tmp);
        tmp = NULL;
    }
    infoGame("Player removed succesfully!");
    return 0;
}

int movePlayer(Game* game, Cell *player, int direction) {
    char logMessage[2000];
    sprintf(logMessage, "Moving player '%s' to %d", player->user, direction);
    infoGame(logMessage);

    int status = ERR_PLAYER_MOVED_SUCCESS;

    Cell *movingPlayerCell = NULL;
    Cell *currentCell = game->playerCells;
    while(currentCell != NULL)
    {
        if(strcmp(currentCell->user, player->user) == 0)
        {
            movingPlayerCell = currentCell;
        }
        currentCell = currentCell->next;
    }

    if(movingPlayerCell == NULL) {
        sprintf(logMessage, "Player '%s' not found", player->user);
        warnGame(logMessage);

        return ERR_PLAYER_NOT_FOUND;
    }

    int newX = movingPlayerCell->x;
    int newY = movingPlayerCell->y;

    switch(direction) {
        case MOVE_UP :
            newY = movingPlayerCell->y - 1;
            if(newY < 0) {
                status = ERR_POSITION_OUT_OF_BOUND;
            }
            break;
        case MOVE_RIGHT :
            newX = movingPlayerCell->x + 1;
            if(newX > game->cols - 1) {
                status = ERR_POSITION_OUT_OF_BOUND;
            }
            break;
        case MOVE_DOWN :
            newY = movingPlayerCell->y + 1;
            if(newY > game->rows - 1) {
                status = ERR_POSITION_OUT_OF_BOUND;
            }
            break;
        case MOVE_LEFT :
            newX = movingPlayerCell->x - 1;
            if(newX < 0) {
                status = ERR_POSITION_OUT_OF_BOUND;
            }
            break;
        default :
            status = ERR_INVALID_DIRECTION;
    }

    if(status == ERR_PLAYER_MOVED_SUCCESS)
    {
        //Now checks if the player hits the bomb
        if(hasBombAt(game, newX, newY))
        {
            warnGame("Player hit a bomb. Blew up!");
            status = ERR_PLAYER_HIT_BOMB;
        }
        else if(hasPlayerAt(game, newX, newY) == 1)
        {
            warnGame("Player hit a busy cell.");
            status = ERR_CELL_BUSY;
            newX = movingPlayerCell->x;
            newY = movingPlayerCell->y;
        }
        //In this case the user won the game
        else if(newX == game->cols - 1) {
            infoGame("Player won the game!");
            status = ERR_USER_WIN_GAME;
        }
        else {
            infoGame("Player moved succesfully!");
        }
        movingPlayerCell->x = newX;
        movingPlayerCell->y = newY;
    }

    return status;
}

Cell* generateRandomPlayerCell(Player *player)
{
    char logMessage[2000];
    sprintf(logMessage, "Generating random position for player '%s'", player->username);
    infoGame(logMessage);

    Game *game = getCurrentGame();
    Cell *cell = NULL;
    if(game != NULL)
    {
        int y;
        do
        {
            y = rand() % (game->rows - 1);
        }
        while(hasPlayerAt(game, 0, y));

        cell = (Cell*) malloc(sizeof(Cell));
        cell->user = player->username;
        cell->x = 0;
        cell->y = y;
        cell->symbol = player->symbol;
        cell->color = player->color;
        cell->next = NULL;
    }

    sprintf(logMessage, "Player positioned at x=%d, y=%d", cell->x, cell->y);
    infoGame(logMessage);

    return cell;
}
