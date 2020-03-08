#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../../include/game.h"
#include "../../include/player.h"

#define NUM_ROWS 20
#define NUM_COLS 30

Game *currentGame = NULL;

void deployRandomBombs(Game *game) {

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
    game->numBombs = numBombs;
    game->bombCells = headCell;
}

Cell* generateRandomPlayerCell(Player *player)
{
    Cell *cell = (Cell*) malloc(sizeof(Cell));
    cell->user = player->username;
    cell->x = 0;
    cell->y = 5; //Random
    cell->symbol = player->symbol;
    cell->color = player->color;
    cell->next = NULL;

    return cell;
}

Game* generateNewGame() {

    Game *game = (Game*) malloc(sizeof(Game));
    game->rows = NUM_ROWS;
    game->cols = NUM_COLS;
    game->playerCells = NULL;
    game->bombCells = NULL;

    Player *players = getConnectedPlayers();

    int count = 0;
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
        count++;
    }
    game->playerCells = headCell;
    game->numPlayers = count;

    deployRandomBombs(game);

    if(currentGame != NULL)
    {
        free(currentGame);
    }

    currentGame = game;
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
    if(game->numPlayers == game->rows) {
        return ERR_MAX_NUMBER_OF_PLAYER_REACHED;
    }

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
    game->numPlayers++;

    return 0;
}

int removePlayer(Game* game, Cell *player) {
    int hasPlayer = hasPlayerAt(game, player->x, player->y);
    if(!hasPlayer) {
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

        game->numPlayers--;
        free(tmp);
        tmp = NULL;
    }

    return 0;
}

int movePlayer(Game* game, Cell *player, int direction) {
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
        return ERR_PLAYER_NOT_FOUND;
    }

    switch(direction) {
        case MOVE_UP :
            if(movingPlayerCell->y < 0) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            movingPlayerCell->y = movingPlayerCell->y - 1;
            break;
        case MOVE_RIGHT :
            if(movingPlayerCell->x > game->cols - 1) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            movingPlayerCell->x = movingPlayerCell->x + 1;
            break;
        case MOVE_DOWN :
            if(movingPlayerCell->y > game->rows - 1) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            movingPlayerCell->y = movingPlayerCell->y + 1;
            break;
        case MOVE_LEFT :
            if(movingPlayerCell->x < 0) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            movingPlayerCell->x = movingPlayerCell->x - 1;
            break;
        default :
            return ERR_INVALID_DIRECTION;
   }

    //Now checks if the player hits the bomb
    if(hasBombAt(game, movingPlayerCell->x, movingPlayerCell->y)) {
       return ERR_PLAYER_HIT_BOMB;
    }
    //In this case the user won the game
    else if(movingPlayerCell->x == game->cols - 1) {
        return ERR_USER_WIN_GAME;
    }

   return ERR_PLAYER_MOVED_SUCCESS;
}
