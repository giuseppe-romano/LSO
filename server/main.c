#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct coordinate {
    int x;
    int y;
} Coordinate;

void drawCellEmpty() {
    printf("|   ");
}

void drawCellWithBomb() {
    printf("|");
    printf("\033[1;31m");
    printf(" X ");
    printf("\033[0m");
}

void deployBombs(Coordinate bombCoordinates[], int numRows, int numCols) {

    int j;

    for( j = 0 ; j < 2 * (numRows + numCols) ; j++ ) {
        Coordinate tmp;
        tmp.x = (rand() % numRows) + 1;
        tmp.y = rand() % numCols;

        bombCoordinates[j] = tmp;
    }
}

int containsBomb(Coordinate bombCoordinates[], int numBombs, int x, int y) {
    int res = 0;
    int j;
    for(j = 0; j < numBombs && res == 0; j++) {
   // printf("coordinate x=%d   y=%d\n", bombCoordinates[j].x, bombCoordinates[j].y);
        if(bombCoordinates[j].x == x && bombCoordinates[j].y == y) {
            res = 1;
        }
    }
    return res;
}

void drawMatrix(int numRows, int numCols, Coordinate bombCoordinates[], int numBombs) {
    int i, j;

    for(i = 0; i < numCols; i++) {
        printf("\t\t\t");
        for(j = 0; j < numRows; j++) {
            printf("----");
        }
        printf("\n\t\t\t");
        for(j = 0; j < numRows; j++) {
            if(containsBomb(bombCoordinates, numBombs, j, i)) {
                drawCellWithBomb();
            }
            else {
                drawCellEmpty();
            }
        }
        printf("|\n");
    }
    printf("\t\t\t");
    for(j = 0; j < numRows; j++) {
        printf("----");
    }

    printf("\n");
}

int main()
{
printf("Normal text\n");
system("setterm -bold on");
printf("Bold text\n");
system("setterm -bold off");

    int NUM_ROWS = 30;
    int NUM_COLS = 20;

    time_t t;
     /* Intializes random number generator */
    srand((unsigned) time(&t));

    int numBombs = 2 * (NUM_ROWS + NUM_COLS);
    Coordinate bombCoordinates[numBombs];
    deployBombs(bombCoordinates, NUM_ROWS, NUM_COLS);
    Coordinate playerCoordinates[15];

    int j;

   // size_t numBombs = sizeof(&bombCoordinates)/sizeof(&(bombCoordinates[0]));
    for(j = 0; j < numBombs; j++) {
     //   printf("coordinate x=%d   y=%d\n", &(bombCoordinates[j]).x, &(bombCoordinates[j]).y);

    }

    size_t numPlayers = sizeof(playerCoordinates)/sizeof(playerCoordinates[0]);
    for(j = 0; j < numPlayers; j++) {
        Coordinate tmp;
        tmp.x = j + 1;
        tmp.y = j + 1;

        playerCoordinates[j] = tmp;
    }

   // printf("\033[1;31m");
    drawMatrix(NUM_ROWS, NUM_COLS, &bombCoordinates, numBombs);
//  printf("\033[0m;");

  return 0;
}
