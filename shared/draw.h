#ifndef DRAW_HEADERFILE_INCLUDED
#define DRAW_HEADERFILE_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "serial.h"
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void setInteractiveCursorCoords(int x, int y);
void drawServerTitle();
void drawClientTitle();
void drawMineField(Game *game);
void drawPlayer(Player *player);
void drawConnectedPlayer(Cell *player);
void printNotificationMessage(int notificationStatus, char *notificationMessage);

#endif
