#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../include/draw.h"

void drawCell(Cell *cell) {
    printf("|");

    if(cell == NULL) {
        printf("   ");
    }
    else {
        printf("%s", cell->color);
        printf(" %c ", cell->symbol);
    }
    printf("\033[0m");
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

void drawTitle() {
    system("setterm -bold on");

    printf("\n");
    printf("\t\t\t");
    printf("Il campo minato");
    printf("\t\t\t");
    printf("\n");

    system("setterm -bold off");
}

void drawMineField(Game *game) {
    int i, j;

    for(i = 0; i < game->rows; i++) {
        printf("\t\t\t");
        for(j = 0; j < game->cols; j++) {
            printf("----");
        }
        printf("\n\t\t\t");
        for(j = 0; j < game->cols; j++) {
            Cell *cell = getCellAt(game->bombCells, game->numBombs, j, i);
            if(cell == NULL) {
                cell = getCellAt(game->playerCells, game->numPlayers, j, i);
            }

            drawCell(cell);
        }
        printf("|\n");
    }
    printf("\t\t\t");
    for(j = 0; j < game->cols; j++) {
        printf("----");
    }

    printf("\n");
}

void drawScreen(Game *game) {
    system("@cls||clear");

    drawTitle();
    drawMineField(game);
}
