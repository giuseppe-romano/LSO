#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serial.h"
#include "logging.h"

#define ROWS_TOKEN "rows="
#define COLS_TOKEN "cols="
#define NUM_PLAYERS_TOKEN "numPlayers="
#define PLAYER_TOKEN "player="
#define X_TOKEN "x="
#define Y_TOKEN "y="
#define SYMBOL_TOKEN "symbol="
#define USER_TOKEN "user="
#define COLOR_TOKEN "color="
#define DIRECTION_TOKEN "direction="
#define PWD_TOKEN "password="

#define NEW_GAME_ACTION "GAME<"
#define MOVE_PLAYER_ACTION "MOVE_PLAYER<"
#define LOGIN_ACTION "LOGIN<"
#define REGISTER_ACTION "REGISTER<"

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

char *substring(char *string, int position, int length)
{
   char *pointer;
   int c;

   pointer = malloc(length+1);

   if (pointer == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(1);
   }

   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *(string+position-1);
      string++;
   }

   *(pointer+c) = '\0';

   return pointer;
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
    strcpy(message, NEW_GAME_ACTION);
    strcat(message, ROWS_TOKEN);
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
        strcat(message, PLAYER_TOKEN);

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
    strcat(message, ">");

    return message;
}

Game* deserializeGame(char *string)
{
    Game *game = (Game*) malloc(sizeof(Game));

    char *data = substring(string, strlen(NEW_GAME_ACTION) + 1, strlen(string) - strlen(NEW_GAME_ACTION) - 1);

    Cell *player;
    char *tokens = strtok(data, "|");

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
        else if(strStartWith(tokens, PLAYER_TOKEN)) {
            player = getCellValue(tokens, PLAYER_TOKEN);

            game->playerCells[playerCounter++] = *player;
        }
        tokens = strtok(NULL, "|");
    }

   return game;
}

char* serializeMovePlayerAction(Cell *player, char *direction)
{
    char logging[2000];
    sprintf(logging, "Serializing move player action with direction '%s'...", direction);
    info(logging);
    char buffer[200];

    char *message = malloc(2000);
    strcpy(message, MOVE_PLAYER_ACTION);
    strcat(message, PLAYER_TOKEN);

    sprintf(buffer, "{x=%d,y=%d,symbol=%s,color=%s,user=%s,}",
        player->x,
        player->y,
        player->symbol,
        player->color,
        player->user);
    strcat(message, buffer);

    strcat(message, "|");
    strcat(message, DIRECTION_TOKEN);
    strcat(message, direction);
    strcat(message, ">");


    sprintf(logging, "Move action serialized %s", message);
    info(logging);

    return message;
}

MovePlayerAction* deserializeMovePlayerAction(char *string)
{
    if(!strStartWith(string, MOVE_PLAYER_ACTION)) {
        return NULL;
    }

    MovePlayerAction *action = (MovePlayerAction*) malloc(sizeof(MovePlayerAction));

    char *data = substring(string, strlen(MOVE_PLAYER_ACTION) + 1, strlen(string) - strlen(MOVE_PLAYER_ACTION) - 1);
    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, PLAYER_TOKEN)) {
            action->player = getCellValue(tokens, PLAYER_TOKEN);
        }
        else if(strStartWith(tokens, DIRECTION_TOKEN)) {
            action->direction = getStringValue(tokens, DIRECTION_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

   return action;
}

char* serializeLoginAction(char *username, char *password)
{
    char logging[2000];
    sprintf(logging, "Serializing login action with username '%s'...", username);
    info(logging);

    char *message = malloc(2000);
    strcpy(message, LOGIN_ACTION);
    strcat(message, USER_TOKEN);
    strcat(message, username);
    strcat(message, "|");
    strcat(message, PWD_TOKEN);
    strcat(message, password);
    strcat(message, ">");


    sprintf(logging, "Login action serialized %s", message);
    info(logging);

    return message;
}

AuthenticationAction* deserializeLoginAction(char *string)
{
    if(!strStartWith(string, LOGIN_ACTION)) {
        return NULL;
    }

    AuthenticationAction *action = (AuthenticationAction*) malloc(sizeof(AuthenticationAction));

    char *data = substring(string, strlen(LOGIN_ACTION) + 1, strlen(string) - strlen(LOGIN_ACTION) - 1);
    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            action->username = getStringValue(tokens, USER_TOKEN);
        }
        else if(strStartWith(tokens, PWD_TOKEN)) {
            action->password = getStringValue(tokens, PWD_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

   return action;
}

char* serializeRegisterAction(char *username, char *password)
{
    char logging[2000];
    sprintf(logging, "Serializing register action with username '%s'...", username);
    info(logging);

    char *message = malloc(2000);
    strcpy(message, REGISTER_ACTION);
    strcat(message, USER_TOKEN);
    strcat(message, username);
    strcat(message, "|");
    strcat(message, PWD_TOKEN);
    strcat(message, password);
    strcat(message, ">");


    sprintf(logging, "Register action serialized %s", message);
    info(logging);

    return message;
}

AuthenticationAction* deserializeRegisterAction(char *string)
{
    if(!strStartWith(string, REGISTER_ACTION)) {
        return NULL;
    }

    AuthenticationAction *action = (AuthenticationAction*) malloc(sizeof(AuthenticationAction));

    char *data = substring(string, strlen(REGISTER_ACTION) + 1, strlen(string) - strlen(REGISTER_ACTION) - 1);
    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            action->username = getStringValue(tokens, USER_TOKEN);
        }
        else if(strStartWith(tokens, PWD_TOKEN)) {
            action->password = getStringValue(tokens, PWD_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

   return action;
}
