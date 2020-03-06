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

    //Defines the list of bombs
    game->numBombs = numBombs;
    game->bombCells = (Cell*) malloc(numBombs * sizeof(Cell));

    int j;
    for( j = 0 ; j < numBombs; j++ ) {
        Cell *b = (Cell*) malloc(sizeof(Cell));
        int x = rand() % game->cols;
        b->x = x == 0 ? x + 1 : ((x == game->cols - 1) ? x - 1 : x);
        b->y = rand() % game->rows;
        b->symbol = "X";
        b->color = "red";

        game->bombCells[j] = *b;
    }
}

Cell* generateRandomPlayerCell(Player *player)
{
    Cell *cell = (Cell*) malloc(sizeof(Cell));
    cell->user = player->username;
    cell->x = 0;
    cell->y = 5; //Random
    cell->symbol = player->symbol;
    cell->color = player->color;

    return cell;
}

Game* generateNewGame() {

    Game *game = (Game*) malloc(sizeof(Game));
    game->rows = NUM_ROWS;
    game->cols = NUM_COLS;

    Player *players = getConnectedPlayers();

    //Allocate the maximum number of players (one for each row)
    game->playerCells = (Cell*) malloc(game->rows * sizeof(Cell));

    int count = 0;
    Player *current = players;
    while (current != NULL) {
        game->playerCells[count++] = *generateRandomPlayerCell(current);
        current = current->next;
    }
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

int indexOfPlayer(Game* game, Cell *player) {
    int index = -1;
    int j;
    for(j = 0; j < game->numPlayers; j++) {
        if(game->playerCells[j].x == player->x && game->playerCells[j].y == player->y) {
            index = j;
        }
    }
    return index;
}

Cell* getPlayerByUsername(Game *game, char *username) {
    Cell player;
    Cell *cell = NULL;
    int j;
    int found = 0;
    for(j = 0; j < game->numPlayers; j++) {
        if(strcmp(game->playerCells[j].user, username) == 0) {
            player = game->playerCells[j];
            found = 1;
        }
    }

    if(found)
    {
        cell = (Cell*) malloc(sizeof(Cell));
        cell->user = username;
        cell->x = player.x;
        cell->y = player.y;
        cell->symbol = player.symbol;
        cell->color = player.color;
    }
    return cell;
}

int hasBombAt(Game* game, int x, int y) {
    int j;
    for(j = 0; j < game->numBombs; j++) {
        if(game->bombCells[j].x == x && game->bombCells[j].y == y) {
            return 1;
        }
    }
    return 0;
}

int addPlayer(Game* game, Cell *player) {
    //First checks if the maximum number of player has been reached
    if(game->numPlayers == game->rows) {
        return ERR_MAX_NUMBER_OF_PLAYER_REACHED;
    }

    int j;
    for(j = 0; j < game->numPlayers; j++) {
        if(game->playerCells[j].x == player->x && game->playerCells[j].y == player->y) {
            return ERR_CELL_BUSY;
        }
    }

    game->playerCells[game->numPlayers] = *player;
    game->numPlayers++;

    return 0;
}

int removePlayer(Game* game, Cell *player) {
    int j;
    int index = indexOfPlayer(game, player);
    if(index == -1) {
        return ERR_PLAYER_NOT_FOUND;
    }

    int shiftFrom = index + 1;
    for(j = shiftFrom; j < game->numPlayers; j++) {
        game->playerCells[j - 1] = game->playerCells[j];
    }

    game->numPlayers--;

    return 0;
}

int movePlayer(Game* game, Cell *player, int direction) {
    int index = indexOfPlayer(game, player);
    if(index == -1) {
        return ERR_PLAYER_NOT_FOUND;
    }

    switch(direction) {
        case MOVE_UP :
            if(game->playerCells[index].y < 0) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            game->playerCells[index].y--;
            break;
        case MOVE_RIGHT :
            if(game->playerCells[index].x > game->cols - 1) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            game->playerCells[index].x++;
            break;
        case MOVE_DOWN :
            if(game->playerCells[index].y > game->rows - 1) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            game->playerCells[index].y++;
            break;
        case MOVE_LEFT :
            if(game->playerCells[index].x < 0) {
                return ERR_POSITION_OUT_OF_BOUND;
            }
            game->playerCells[index].x--;
            break;
        default :
            return ERR_INVALID_DIRECTION;
   }

    //Now checks if the player hits the bomb
    if(hasBombAt(game, game->playerCells[index].x, game->playerCells[index].y)) {
       return ERR_PLAYER_HIT_BOMB;
    }
    //In this case the user won the game
    else if(game->playerCells[index].x == game->cols - 1) {
        return ERR_USER_WIN_GAME;
    }

   return 0;
}
