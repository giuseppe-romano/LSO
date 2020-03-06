#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "draw.h"

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

Cell* getCellAt(Cell cells[], int numCells, int x, int y) {
    Cell *res = NULL;

    int j;
    for(j = 0; j < numCells; j++) {
        if(cells[j].x == x && cells[j].y == y) {
            res = &cells[j];
        }
    }
    return res;
}

void drawServerTitle() {
    system("setterm -bold on");

    gotoxy(91, 2);
    printf("M I N E F I E L D   G A M E   -   S E R V E R   C O N S O L E");

    system("setterm -bold off");
}

void drawClientTitle() {
    system("setterm -bold on");

    gotoxy(91, 2);
    printf("M I N E F I E L D   G A M E   -   C L I E N T   C O N S O L E");

    system("setterm -bold off");
}

void drawMineField(Game *game) {
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
                cell = getCellAt(game->bombCells, game->numBombs, j, i);
            }
            if(cell == NULL) {
                cell = getCellAt(game->playerCells, game->numPlayers, j, i);
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
}

void drawPlayer(Player *player)
{
    char m[2000];
    printf("%s", getColorByName(player->color));
    sprintf(m, "    username: %s,  password: %s, symbol:%s", player->username, player->password, player->symbol);

    printf("%-59s", m);
    printf(RESET);
}
