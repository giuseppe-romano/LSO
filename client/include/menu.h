#ifndef MENUCLIENT_HEADERFILE_INCLUDED
#define MENUCLIENT_HEADERFILE_INCLUDED

void *menuThreadFunc(void *vargp);

void setUserLoggedIn(int status);

void showMainMenu();
void showRegisterMenu();
void showLoginMenu();
void showGameMenu();

#endif
