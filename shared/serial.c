#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serial.h"

#define ROWS_TOKEN "rows="
#define COLS_TOKEN "cols="
#define NUM_PLAYERS_TOKEN "numPlayers="
#define PLAYER_CELL_TOKEN "playerCell="
#define X_TOKEN "x="
#define Y_TOKEN "y="
#define SYMBOL_TOKEN "symbol="
#define USER_TOKEN "user="
#define COLOR_TOKEN "color="

int strStartWith(char *string, char *prefix)
{
    if(strncmp(string, prefix, strlen(prefix)) == 0)
    {
        return 1;
    }
    return 0;
}

int strIndexOf(char *string, char delim)
{
    char *pos = strchr (string, delim);
    int index = pos ? pos - string : -1;
    return index;
}

char* getStringValue(char *string, char *prefix)
{
    char *val = string + strlen(prefix);

    char *ret = malloc(strlen(val));
    strcpy(ret, val);

    return ret;
}

int getIntValue(char *string, char *prefix)
{
    char *val = string + strlen(prefix);

    int ret;
    sscanf(val, "%d", &ret);
    return ret;
}

Cell* getCellValue(char *string, char *prefix)
{
    Cell *cell = malloc(sizeof(Cell));
    char *val = string + strlen(prefix) + 1;

    //remove the last }
    val[strlen(val) - 1] = '\0';

    char token[50];
    int index = strIndexOf(val, ',');
    while(index != -1) {

        memset(token, '\0', 50);
        strncpy(token, val, index);

        if(strStartWith(token, X_TOKEN)) {
            cell->x = getIntValue(token, X_TOKEN);
        }
        else if(strStartWith(token, Y_TOKEN)) {
            cell->y = getIntValue(token, Y_TOKEN);
        }
        else if(strStartWith(token, SYMBOL_TOKEN)) {
            cell->symbol = getStringValue(token, SYMBOL_TOKEN);
        }
        else if(strStartWith(token, USER_TOKEN)) {
            cell->user = getStringValue(token, USER_TOKEN);
        }
        else if(strStartWith(token, COLOR_TOKEN)) {
            cell->color = getStringValue(token, COLOR_TOKEN);
        }

        val = val + index + 1;
        index = strIndexOf(val, ',');
    }

    return cell;
}

char* serializeGame(Game *game)
{
    char str[10];

    char *message = malloc(2000);
    strcpy(message, ROWS_TOKEN);
    sprintf(str, "%d", game->rows);
    strcat(message, str);
    strcat(message, "|");

    strcat(message, COLS_TOKEN);
    sprintf(str, "%d", game->cols);
    strcat(message, str);
    strcat(message, "|");

    strcat(message, NUM_PLAYERS_TOKEN);
    sprintf(str, "%d", game->numPlayers);
    strcat(message, str);
    strcat(message, "|");

    char buffer[100];
    int j;
    for( j = 0 ; j < game->numPlayers; j++ ) {
        strcat(message, PLAYER_CELL_TOKEN);

        sprintf(buffer, "{x=%d,y=%d,symbol=%s,color=%s,user=%s,}",
            game->playerCells[j].x,
            game->playerCells[j].y,
            game->playerCells[j].symbol,
            game->playerCells[j].color,
            game->playerCells[j].user);
        strcat(message, buffer);

        if(j < game->numPlayers - 1) {
            strcat(message, "|");
        }
    }

    return message;
}

Game* deserializeGame(char *string)
{
    Game *game = (Game*) malloc(sizeof(Game));

    Cell *player;
    char *tokens = strtok(string, "|");

    int playerCounter = 0;
    while( tokens != NULL ) {
        if(strStartWith(tokens, ROWS_TOKEN)) {
            game->rows = getIntValue(tokens, ROWS_TOKEN);

            //Allocate the maximum number of players (one for each row)
            game->playerCells = (Cell*) malloc(game->rows * sizeof(Cell));
        }
        else if(strStartWith(tokens, COLS_TOKEN)) {
            game->cols = getIntValue(tokens, COLS_TOKEN);
        }
        else if(strStartWith(tokens, NUM_PLAYERS_TOKEN)) {
            game->numPlayers = getIntValue(tokens, NUM_PLAYERS_TOKEN);
        }
        else if(strStartWith(tokens, PLAYER_CELL_TOKEN)) {
            player = getCellValue(tokens, PLAYER_CELL_TOKEN);

            game->playerCells[playerCounter++] = *player;
        }
        tokens = strtok(NULL, "|");
    }

   return game;
}

