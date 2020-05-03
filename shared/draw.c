#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "draw.h"
#include "logging.h"

int currentXInputOffset = 0;
int currentYInputOffset = 0;

void setInteractiveCursorCoords(int x, int y)
{
    currentXInputOffset = x;
    currentYInputOffset = y;
}

void setCursorToOffset()
{
    char logMessage[2000];
    sprintf(logMessage, "Set cursor to x=%d, y=%d", currentXInputOffset, currentYInputOffset);
    infoDraw(logMessage);

    gotoxy(currentXInputOffset, currentYInputOffset);
}


char* getColorByName(char *name)
{
    if (strcmp(name, "red") == 0)
    {
        return RED;
    }
    else if (strcmp(name, "green") == 0)
    {
        return GRN;
    }
    else if (strcmp(name, "yellow") == 0)
    {
        return YEL;
    }
    else if (strcmp(name, "blue") == 0)
    {
        return BLU;
    }
    else if (strcmp(name, "magenta") == 0)
    {
        return MAG;
    }
    else if (strcmp(name, "ciano") == 0)
    {
        return CYN;
    }
    else if (strcmp(name, "white") == 0)
    {
        return WHT;
    }
    else /* default: */
    {
        return RESET;
    }
}

void drawCell(Cell *cell, int x, int y) {
    gotoxy(x, y);
    printf("|");

    if(cell == NULL) {
        printf("   ");
    }
    else {
        printf("%s", getColorByName(cell->color));
        printf(" %s ", cell->symbol);
        printf(RESET);
    }
}

Cell* getCellAt(Cell *head, int x, int y) {
    Cell *res = NULL;
    Cell *current = head;
    while (current != NULL && res == NULL) {
        if(current->x == x && current->y == y) {
            res = current;
        }

        current = current->next;
    }
    return res;
}

void drawServerTitle() {
    infoDraw("Drawing Server title...");
    system("setterm -bold on");

    gotoxy(91, 2);
    printf("M I N E F I E L D   G A M E   -   S E R V E R   C O N S O L E");

    system("setterm -bold off");
    infoDraw("Server title drawn!");
    setCursorToOffset();
}

void drawClientTitle() {
    infoDraw("Drawing Client title...");
    system("setterm -bold on");

    gotoxy(91, 2);
    printf("M I N E F I E L D   G A M E   -   C L I E N T   C O N S O L E");

    system("setterm -bold off");
    infoDraw("Client title drawn!");
    setCursorToOffset();
}

void drawMineField(Game *game) {
    infoDraw("Drawing Minefield...");
    int i, j;

    int yOffset = 5;
    int xOffset = 60;
    for(i = 0; i < game->rows; i++) {
        gotoxy(xOffset, yOffset);
        for(j = 0; j < game->cols; j++) {
            printf("----");
        }

        yOffset += 2;
        gotoxy(xOffset, yOffset);
        for(j = 0; j < game->cols; j++) {
            Cell *cell = NULL;
            if(game->bombCells) {
                cell = getCellAt(game->bombCells, j, i);
            }
            if(cell == NULL) {
                cell = getCellAt(game->playerCells, j, i);
            }

            drawCell(cell, xOffset + (j * 4), yOffset -1);
        }
        printf("|");
    }
    gotoxy(xOffset, yOffset);

    for(j = 0; j < game->cols; j++) {
        printf("----");
    }

    printf("\n");
    infoDraw("Minefield drawn!");
    setCursorToOffset();
}

void drawPlayer(Player *player)
{
    infoDraw("Drawing Player...");
    char m[2000];
    printf("%s", getColorByName(player->color));
    sprintf(m, "    username: %s,  password: %s, symbol:%s", player->username, player->password, player->symbol);

    printf("%-59s", m);
    printf(RESET);
    infoDraw("Player drawn!");
    setCursorToOffset();
}

void printNotificationMessage(int notificationStatus, char *notificationMessage)
{
    if(notificationMessage)
    {
        infoDraw("Drawing Notification message...");
        gotoxy(1, 20);
        printf(GRN);
        if(notificationStatus != 0)
        {
            printf(RED);
        }
        printf("%-59s", notificationMessage);
        printf(RESET);
        infoDraw("Notification message drawn!");
    }
    setCursorToOffset();
}

void drawConnectedPlayer(Cell *player)
{
    gotoxy(60, 4);
    printf("%-100s", " ");
    if(player != NULL)
    {
        infoDraw("Drawing Connected Player...");

        gotoxy(60, 4);
        printf("Welcome %s (", player->user);
        printf("%s", getColorByName(player->color));
        printf("%s", player->symbol);
        printf(RESET);
        printf(")");
        infoDraw("Connected Player drawn!");
    }
    setCursorToOffset();
}


