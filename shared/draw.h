#ifndef DRAW_HEADERFILE_INCLUDED
#define DRAW_HEADERFILE_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "serial.h"
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

void drawScreen(Game *game);

#endif
