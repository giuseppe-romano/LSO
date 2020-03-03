#ifndef MENUCLIENT_HEADERFILE_INCLUDED
#define MENUCLIENT_HEADERFILE_INCLUDED

void *menuThreadFunc(void *vargp);

void setRegisterResponseReceived(int status, char *message);
void setLoginResponseReceived(int status, char *message);

void showMainMenu();
void showRegisterMenu();
void showLoginMenu();
void showGameMenu();

#endif
