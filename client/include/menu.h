#ifndef MENUCLIENT_HEADERFILE_INCLUDED
#define MENUCLIENT_HEADERFILE_INCLUDED
#include "serial.h"

void *menuThreadFunc(void *vargp);

void setRegisterResponseReceived(int status, char *message);
void setLoginResponseReceived(int status, char *message);
void setCurrentPlayerCell(Cell *playerCell);

void showMainMenu();
void showRegisterMenu();
void showLoginMenu();
void showGameMenu();

void setCursorToOffset();

#endif
