#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../../include/protocol.h"

char* serializeGame(Game *game)
{
    char str[10];

    char *message = malloc(2000);
    strcpy(message, "rows=");
    sprintf(str, "%d", game->rows);
    strcat(message, str);
    strcat(message, "|");

    strcat(message, "cols=");
    sprintf(str, "%d", game->cols);
    strcat(message, str);
    strcat(message, "|");

    strcat(message, "numPlayers=");
    sprintf(str, "%d", game->numPlayers);
    strcat(message, str);
    strcat(message, "|");

    char buffer[50];
    int j;
    for( j = 0 ; j < game->numPlayers; j++ ) {
        strcat(message, "playerCells=");

        sprintf(buffer, "{x=%d,y=%d,symbol=%c,color=%s}", game->playerCells[j].x, game->playerCells[j].y, game->playerCells[j].symbol, game->playerCells[j].color);
        strcat(message, buffer);

        if(j < game->numPlayers - 1) {
            strcat(message, "|");
        }
    }

    return message;
}

Game* deserializeGame(char *string)
{
    printf( "string %d\n", strlen(string) ); //printing each token
 /*   char s[] = "hello|wordl";
    char *tokens = strtok(*string, "|");

    while( tokens != NULL ) {
      printf( " %s\n", tokens ); //printing each token
      tokens = strtok(NULL, "|");
   }*/
}

void sendNewGame(int clientSockets[], Game *game)
{
    char *message = serializeGame(game);

    int j;
    for( j = 0 ; j < game->numPlayers; j++ ) {
        if( send(clientSockets[j] , message , strlen(message) , 0) < 0)
        {
            printf("Send failed\n");
        }
    }

    free(message);
}
