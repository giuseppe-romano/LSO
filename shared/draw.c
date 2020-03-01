#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "draw.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void drawCell(Cell *cell, int x, int y) {
    gotoxy(x, y);
    printf("|");

    if(cell == NULL) {
        printf("   ");
    }
    else {
        printf("%s", cell->color);
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

