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
#define STATUS_TOKEN "status="
#define MESSAGE_TOKEN "message="

#define NEW_GAME_ACTION "GAME<"
#define ADDED_CELL_ACTION "ADDED_CELL<"
#define REMOVED_CELL_ACTION "REMOVED_CELL<"
#define LOGIN_REQUEST "LOGIN<"
#define LOGOUT_REQUEST "LOGOUT<"
#define REGISTER_REQUEST "REGISTER<"
#define REGISTER_RESPONSE "REGISTER_RESPONSE<"
#define LOGIN_RESPONSE "LOGIN_RESPONSE<"
#define MOVE_PLAYER_REQUEST "MOVE_PLAYER<"
#define MOVE_PLAYER_RESPONSE "MOVE_PLAYER_RESPONSE<"

/**
 * This function checks if a string starts with a specified substring.
 * It returns 1 if the string starts with, 0 otherwise
 */
int strStartWith(char *string, char *prefix)
{
    if(strncmp(string, prefix, strlen(prefix)) == 0)
    {
        return 1;
    }
    return 0;
}

/**
 * This function gets the index of a specified character
 * It returns the char position if present, -1 otherwise
 */
int strIndexOf(char *string, char delim)
{
    char *pos = strchr (string, delim);
    int index = pos ? pos - string : -1;
    return index;
}

/**
 * This function extracts the substring starting at position for 'length' characters
 * It returns the substring
 */
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

/**
 * This function extracts the remaining part of the string.
 * Example: getStringValue("prop=value", "prop=") will return "value"
 */
char* getStringValue(char *string, char *prefix)
{
    char *val = string + strlen(prefix);

    char *ret = malloc(strlen(val));
    strcpy(ret, val);

    return ret;
}

/**
 * This function extracts the remaining part of the string and convert it as integer value.
 * Example: getIntValue("prop=123", "prop=") will return 123 as int
 */
int getIntValue(char *string, char *prefix)
{
    char *val = string + strlen(prefix);

    int ret;
    sscanf(val, "%d", &ret);
    return ret;
}

/**
 * This function extracts the Cell struct values from a serialized string.
 * Example: getCellValue("prop={x=1,y=2,symbol=G,user=peppe,color=RED}", "prop=") will return the Cell struct
 */
Cell* getCellValue(char *string, char *prefix)
{
    Cell *cell = malloc(sizeof(Cell));
    cell->next = NULL;
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

/**
 * This function serializes the Game struct to a string using the | (pipe) delimiter.
 * Example: GAME<rows=20|cols=30|player={x=0,y=6,symbol=G,color=red,user=peppe,}>
 */
char* serializeGame(Game *game)
{

    infoSerial("Serializing game...");

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

    Cell *current = game->playerCells;
    while (current != NULL) {
        strcat(message, "|");
        strcat(message, PLAYER_TOKEN);

        char buffer[100];
        sprintf(buffer, "{x=%d,y=%d,symbol=%s,color=%s,user=%s,}",
            current->x,
            current->y,
            current->symbol,
            current->color,
            current->user);
        strcat(message, buffer);

        current = current->next;
    }
    strcat(message, ">\n");

    char logging[2000];
    sprintf(logging, "Game serialized '%s'...", message);
    infoSerial(logging);

    return message;
}

/**
 * This function deserializes the string to a Game struct.
 * Example: GAME<rows=20|cols=30|player={x=0,y=6,symbol=G,color=red,user=peppe,}>
 */
Game* deserializeGame(char *string)
{
    if(!strStartWith(string, NEW_GAME_ACTION)) {
        return NULL;
    }

    Game *game = (Game*) malloc(sizeof(Game));
    game->bombCells = NULL;
    game->playerCells = NULL;

    char logging[2000];
    sprintf(logging, "Deserializing game '%s'...", string);
    infoSerial(logging);

    char *data = substring(string, strlen(NEW_GAME_ACTION) + 1, strlen(string) - strlen(NEW_GAME_ACTION) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing game body '%s'...", data);
    infoSerial(logging);

    Cell *currentPlayer = NULL;
    Cell *player;
    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, ROWS_TOKEN)) {
            game->rows = getIntValue(tokens, ROWS_TOKEN);
        }
        else if(strStartWith(tokens, COLS_TOKEN)) {
            game->cols = getIntValue(tokens, COLS_TOKEN);
        }
        else if(strStartWith(tokens, PLAYER_TOKEN)) {
            player = getCellValue(tokens, PLAYER_TOKEN);
            if(currentPlayer == NULL)
            {
                currentPlayer = player;
                game->playerCells = currentPlayer;
            }
            else
            {
                currentPlayer->next = player;
                currentPlayer = currentPlayer->next;
            }
        }
        tokens = strtok(NULL, "|");
    }
    infoSerial("Game deserialized!");

    return game;
}

/**
 * This function serializes the Cell struct to a string using the | (pipe) delimiter.
 * Example: ADDED_CELL<user=user1|x=12|y=23|symbol=S|color=blue>
 */
char* serializeAddedCell(Cell *cell)
{
    infoSerial("Serializing added cell...");

    char *message = malloc(2000);
    strcpy(message, ADDED_CELL_ACTION);

    char buffer[500];
    sprintf(buffer, "%s%s|%s%d|%s%d|%s%s|%s%s",
            USER_TOKEN, cell->user,
            X_TOKEN, cell->x,
            Y_TOKEN, cell->y,
            SYMBOL_TOKEN, cell->symbol,
            COLOR_TOKEN, cell->color);
    strcat(message, buffer);
    strcat(message, ">\n");

    char logging[2000];
    sprintf(logging, "Added cell serialized '%s'...", message);
    infoSerial(logging);

    return message;
}

Cell* deserializeAddedCell(char *string)
{
    if(!strStartWith(string, ADDED_CELL_ACTION)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing added cell '%s'...", string);
    infoSerial(logging);

    Cell *cell = (Cell*) malloc(sizeof(Cell));
    cell->next = NULL;

    char *data = substring(string, strlen(ADDED_CELL_ACTION) + 1, strlen(string) - strlen(ADDED_CELL_ACTION) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing added cell body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            cell->user = getStringValue(tokens, USER_TOKEN);
        }
        else if(strStartWith(tokens, X_TOKEN)) {
            cell->x = getIntValue(tokens, X_TOKEN);
        }
        else if(strStartWith(tokens, Y_TOKEN)) {
            cell->y = getIntValue(tokens, Y_TOKEN);
        }
        else if(strStartWith(tokens, SYMBOL_TOKEN)) {
            cell->symbol = getStringValue(tokens, SYMBOL_TOKEN);
        }
        else if(strStartWith(tokens, COLOR_TOKEN)) {
            cell->color = getStringValue(tokens, COLOR_TOKEN);
        }
        tokens = strtok(NULL, "|");
    }

    infoSerial("Added cell deserialized!");

    return cell;
}

/**
 * This function serializes the Cell struct to a string using the | (pipe) delimiter.
 * Example: REMOVED_CELL<user=user1|x=12|y=23|symbol=S|color=blue>
 */
char* serializeRemovedCell(Cell *cell)
{
    infoSerial("Serializing removed cell...");

    char *message = malloc(2000);
    strcpy(message, REMOVED_CELL_ACTION);

    char buffer[500];
    sprintf(buffer, "%s%s|%s%d|%s%d|%s%s|%s%s",
            USER_TOKEN, cell->user,
            X_TOKEN, cell->x,
            Y_TOKEN, cell->y,
            SYMBOL_TOKEN, cell->symbol,
            COLOR_TOKEN, cell->color);
    strcat(message, buffer);
    strcat(message, ">\n");

    char logging[2000];
    sprintf(logging, "Removed cell serialized '%s'...", message);
    infoSerial(logging);

    return message;
}

Cell* deserializeRemovedCell(char *string)
{
    if(!strStartWith(string, REMOVED_CELL_ACTION)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing removed cell '%s'...", string);
    infoSerial(logging);

    Cell *cell = (Cell*) malloc(sizeof(Cell));
    cell->next = NULL;

    char *data = substring(string, strlen(REMOVED_CELL_ACTION) + 1, strlen(string) - strlen(REMOVED_CELL_ACTION) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing removed cell body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            cell->user = getStringValue(tokens, USER_TOKEN);
        }
        else if(strStartWith(tokens, X_TOKEN)) {
            cell->x = getIntValue(tokens, X_TOKEN);
        }
        else if(strStartWith(tokens, Y_TOKEN)) {
            cell->y = getIntValue(tokens, Y_TOKEN);
        }
        else if(strStartWith(tokens, SYMBOL_TOKEN)) {
            cell->symbol = getStringValue(tokens, SYMBOL_TOKEN);
        }
        else if(strStartWith(tokens, COLOR_TOKEN)) {
            cell->color = getStringValue(tokens, COLOR_TOKEN);
        }
        tokens = strtok(NULL, "|");
    }

    infoSerial("Removed cell deserialized!");
    return cell;
}

/**
 * This function serializes the Player struct to a string using the | (pipe) delimiter.
 * Example: user=user1|password=pwd|symbol=S|color=blue
 */
char* serializePlayer(Player *player)
{
    char logging[2000];
    infoSerial("Serializing player...");

    char *message = malloc(2000);
    strcpy(message, USER_TOKEN);
    strcat(message, player->username);
    strcat(message, "|");
    strcat(message, PWD_TOKEN);
    strcat(message, player->password);
    strcat(message, "|");
    strcat(message, SYMBOL_TOKEN);
    strcat(message, player->symbol);
    strcat(message, "|");
    strcat(message, COLOR_TOKEN);
    strcat(message, player->color);

    sprintf(logging, "Player serialized '%s'", message);
    infoSerial(logging);

    return message;
}

Player* deserializePlayer(char *string)
{
    char logging[2000];
    sprintf(logging, "Deserializing player '%s'...", string);
    infoSerial(logging);

    Player *player = (Player*) malloc(sizeof(Player));
    player->next = NULL;

    char *tokens = strtok(string, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            player->username = getStringValue(tokens, USER_TOKEN);
        }
        else if(strStartWith(tokens, PWD_TOKEN)) {
            player->password = getStringValue(tokens, PWD_TOKEN);
        }
        else if(strStartWith(tokens, SYMBOL_TOKEN)) {
            player->symbol = getStringValue(tokens, SYMBOL_TOKEN);
        }
        else if(strStartWith(tokens, COLOR_TOKEN)) {
            player->color = getStringValue(tokens, COLOR_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

    infoSerial("Player deserialized!");
    return player;
}

/**
 * This function serializes the Player struct to a string using the | (pipe) delimiter.
 * Example: MOVE_PLAYER<player={x=0,y=6,symbol=G,color=red,user=peppe}|direction=10>
 */
char* serializeMovePlayerRequest(Cell *player, int direction)
{
    char logging[2000];
    sprintf(logging, "Serializing move player request with direction '%d'...", direction);
    infoSerial(logging);
    char buffer[200];

    char *message = malloc(2000);
    strcpy(message, MOVE_PLAYER_REQUEST);
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
    sprintf(buffer, "%d", direction);
    strcat(message, buffer);
    strcat(message, ">\n");

    sprintf(logging, "Move player request serialized %s", message);
    infoSerial(logging);

    return message;
}

MovePlayerRequest* deserializeMovePlayerRequest(char *string)
{
    if(!strStartWith(string, MOVE_PLAYER_REQUEST)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing move player request '%s'...", string);
    infoSerial(logging);

    MovePlayerRequest *action = (MovePlayerRequest*) malloc(sizeof(MovePlayerRequest));

    char *data = substring(string, strlen(MOVE_PLAYER_REQUEST) + 1, strlen(string) - strlen(MOVE_PLAYER_REQUEST) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing move player request body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, PLAYER_TOKEN)) {
            action->player = getCellValue(tokens, PLAYER_TOKEN);
        }
        else if(strStartWith(tokens, DIRECTION_TOKEN)) {
            action->direction = getIntValue(tokens, DIRECTION_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

   return action;
}

/**
 * This function serializes the username and password to a string using the | (pipe) delimiter.
 * Example: LOGIN<user=user1|password=pwd>
 */
char* serializeLoginRequest(char *username, char *password)
{
    char logging[2000];
    sprintf(logging, "Serializing login request with username '%s'...", username);
    infoSerial(logging);

    char *message = malloc(2000);
    strcpy(message, LOGIN_REQUEST);
    strcat(message, USER_TOKEN);
    strcat(message, username);
    strcat(message, "|");
    strcat(message, PWD_TOKEN);
    strcat(message, password);
    strcat(message, ">\n");

    sprintf(logging, "Login request serialized %s", message);
    infoSerial(logging);

    return message;
}

AuthenticationRequest* deserializeLoginRequest(char *string)
{
    if(!strStartWith(string, LOGIN_REQUEST)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing login request '%s'...", string);
    infoSerial(logging);

    AuthenticationRequest *action = (AuthenticationRequest*) malloc(sizeof(AuthenticationRequest));
    action->username = NULL;
    action->password = NULL;
    action->color = NULL;
    action->symbol = NULL;

    char *data = substring(string, strlen(LOGIN_REQUEST) + 1, strlen(string) - strlen(LOGIN_REQUEST) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing login request body '%s'...", data);
    infoSerial(logging);

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

    infoSerial("Login request deserialized!");
    return action;
}

/**
 * This function serializes the username to a string using the | (pipe) delimiter.
 * Example: LOGOUT<user=user1>
 */
char* serializeLogoutRequest(char *username)
{
    char logging[2000];
    sprintf(logging, "Serializing logout request with username '%s'...", username);
    infoSerial(logging);

    char *message = malloc(2000);
    strcpy(message, LOGOUT_REQUEST);
    strcat(message, USER_TOKEN);
    strcat(message, username);
    strcat(message, ">\n");

    sprintf(logging, "Logout request serialized %s", message);
    infoSerial(logging);

    return message;
}

AuthenticationRequest* deserializeLogoutRequest(char *string)
{
    if(!strStartWith(string, LOGOUT_REQUEST)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing logout request '%s'...", string);
    infoSerial(logging);

    AuthenticationRequest *action = (AuthenticationRequest*) malloc(sizeof(AuthenticationRequest));
    action->username = NULL;
    action->password = NULL;
    action->color = NULL;
    action->symbol = NULL;

    char *data = substring(string, strlen(LOGOUT_REQUEST) + 1, strlen(string) - strlen(LOGOUT_REQUEST) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing logout request body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            action->username = getStringValue(tokens, USER_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

    infoSerial("Logout request deserialized!");
    return action;
}

/**
 * This function serializes the username and password to a string using the | (pipe) delimiter.
 * Example: REGISTER<user=user1|password=pwd|color=blue|symbol=S>
 */
char* serializeRegisterRequest(char *username, char *password, char *color, char *symbol)
{
    char logging[2000];
    sprintf(logging, "Serializing register request with username '%s'...", username);
    infoSerial(logging);

    char *message = malloc(2000);
    strcpy(message, REGISTER_REQUEST);
    strcat(message, USER_TOKEN);
    strcat(message, username);
    strcat(message, "|");
    strcat(message, PWD_TOKEN);
    strcat(message, password);
    strcat(message, "|");
    strcat(message, COLOR_TOKEN);
    strcat(message, color);
    strcat(message, "|");
    strcat(message, SYMBOL_TOKEN);
    strcat(message, symbol);
    strcat(message, ">\n");

    sprintf(logging, "Register request serialized %s", message);
    infoSerial(logging);

    return message;
}

AuthenticationRequest* deserializeRegisterRequest(char *string)
{
    if(!strStartWith(string, REGISTER_REQUEST)) {
        return NULL;
    }
    char logging[2000];
    sprintf(logging, "Deserializing register request '%s'...", string);
    infoSerial(logging);

    AuthenticationRequest *action = (AuthenticationRequest*) malloc(sizeof(AuthenticationRequest));
    action->username = NULL;
    action->password = NULL;
    action->color = NULL;
    action->symbol = NULL;

    char *data = substring(string, strlen(REGISTER_REQUEST) + 1, strlen(string) - strlen(REGISTER_REQUEST) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing register request body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, USER_TOKEN)) {
            action->username = getStringValue(tokens, USER_TOKEN);
        }
        else if(strStartWith(tokens, PWD_TOKEN)) {
            action->password = getStringValue(tokens, PWD_TOKEN);
        }
        else if(strStartWith(tokens, COLOR_TOKEN)) {
            action->color = getStringValue(tokens, COLOR_TOKEN);
        }
        else if(strStartWith(tokens, SYMBOL_TOKEN)) {
            action->symbol = getStringValue(tokens, SYMBOL_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }
    infoSerial("Register request deserialized!");
    return action;
}

/**
 * This function serializes the username and password to a string using the | (pipe) delimiter.
 * Example: REGISTER_RESPONSE<status=1|message=This is a message>
 */
char* serializeRegisterResponse(int status, char *message)
{
    char logging[2000];
    sprintf(logging, "Serializing register response with status %d, message '%s'...", status, message);
    infoSerial(logging);

    char *response = malloc(2000);
    strcpy(response, REGISTER_RESPONSE);
    strcat(response, STATUS_TOKEN);

    char buffer[10];
    sprintf(buffer, "%d", status);
    strcat(response, buffer);

    strcat(response, "|");
    strcat(response, MESSAGE_TOKEN);
    strcat(response, message);
    strcat(response, ">\n");

    sprintf(logging, "Register response serialized %s", response);
    infoSerial(logging);

    return response;
}

AuthenticationResponse* deserializeRegisterResponse(char *string)
{
    if(!strStartWith(string, REGISTER_RESPONSE)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing register response '%s'...", string);
    infoSerial(logging);

    AuthenticationResponse *action = (AuthenticationResponse*) malloc(sizeof(AuthenticationResponse));
    action->message = NULL;
    action->status = -1;

    char *data = substring(string, strlen(REGISTER_RESPONSE) + 1, strlen(string) - strlen(REGISTER_RESPONSE) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing register response body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, STATUS_TOKEN)) {
            action->status = getIntValue(tokens, STATUS_TOKEN);
        }
        else if(strStartWith(tokens, MESSAGE_TOKEN)) {
            action->message = getStringValue(tokens, MESSAGE_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }
    infoSerial("Register response deserialized!");
    return action;
}

/**
 * This function serializes the status code and the message to a string using the | (pipe) delimiter.
 * Example: LOGIN_RESPONSE<status=1|message=This is a message>
 */
char* serializeLoginResponse(int status, char *message)
{
    char logging[2000];
    sprintf(logging, "Serializing login response with status %d, message '%s'...", status, message);
    infoSerial(logging);

    char *response = malloc(2000);
    strcpy(response, LOGIN_RESPONSE);
    strcat(response, STATUS_TOKEN);

    char buffer[10];
    sprintf(buffer, "%d", status);
    strcat(response, buffer);
    strcat(response, "|");
    strcat(response, MESSAGE_TOKEN);
    strcat(response, message);
    strcat(response, ">\n");

    sprintf(logging, "Login response serialized %s", response);
    infoSerial(logging);

    return response;
}

AuthenticationResponse* deserializeLoginResponse(char *string)
{
    if(!strStartWith(string, LOGIN_RESPONSE)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing login response '%s'...", string);
    infoSerial(logging);

    AuthenticationResponse *action = (AuthenticationResponse*) malloc(sizeof(AuthenticationResponse));
    action->message = NULL;
    action->status = -1;

    char *data = substring(string, strlen(LOGIN_RESPONSE) + 1, strlen(string) - strlen(LOGIN_RESPONSE) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing login response body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, STATUS_TOKEN)) {
            action->status = getIntValue(tokens, STATUS_TOKEN);
        }
        else if(strStartWith(tokens, MESSAGE_TOKEN)) {
            action->message = getStringValue(tokens, MESSAGE_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

    infoSerial("Login response deserialized!");
    return action;
}

/**
 * This function serializes the Cell struct and the status to a string using the | (pipe) delimiter.
 * Example: MOVE_PLAYER_RESPONSE<player={x=0,y=6,symbol=G,color=red,user=peppe,}>
 */
char* serializeMovePlayerResponse(Cell *player, int status)
{
    char logging[2000];
    sprintf(logging, "Serializing move player response with player '%s', status %d...", player->user, status);
    infoSerial(logging);

    char *response = malloc(2000);
    strcpy(response, MOVE_PLAYER_RESPONSE);
    strcat(response, PLAYER_TOKEN);

    char p[2000];
    sprintf(p, "{x=%d,y=%d,symbol=%s,color=%s,user=%s,}",
        player->x,
        player->y,
        player->symbol,
        player->color,
        player->user);
    strcat(response, p);
    strcat(response, "|");

    strcat(response, STATUS_TOKEN);
    char buffer[10];
    sprintf(buffer, "%d", status);
    strcat(response, buffer);
    strcat(response, ">\n");

    sprintf(logging, "Move player response serialized %s", response);
    infoSerial(logging);

    return response;
}

MovePlayerResponse* deserializeMovePlayerResponse(char *string)
{
    if(!strStartWith(string, MOVE_PLAYER_RESPONSE)) {
        return NULL;
    }

    char logging[2000];
    sprintf(logging, "Deserializing move player response '%s'...", string);
    infoSerial(logging);

    MovePlayerResponse *action = (MovePlayerResponse*) malloc(sizeof(MovePlayerResponse));
    action->player = NULL;
    action->status = -1;

    char *data = substring(string, strlen(MOVE_PLAYER_RESPONSE) + 1, strlen(string) - strlen(MOVE_PLAYER_RESPONSE) - 1); //to remove '>' characters
    sprintf(logging, "Deserializing move player response body '%s'...", data);
    infoSerial(logging);

    char *tokens = strtok(data, "|");

    while( tokens != NULL ) {
        if(strStartWith(tokens, STATUS_TOKEN)) {
            action->status = getIntValue(tokens, STATUS_TOKEN);
        }
        else if(strStartWith(tokens, PLAYER_TOKEN)) {
            action->player = getCellValue(tokens, PLAYER_TOKEN);
        }

        tokens = strtok(NULL, "|");
    }

    infoSerial("Move player response deserialized!");
    return action;
}
