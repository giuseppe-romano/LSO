#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "../../include/protocol.h"
#include "draw.h"

int registerResponseReceived = 0;
int registerResponseStatus;
char *registerResponseMessage;

int loginResponseReceived = 0;
int loginResponseStatus;
char *loginResponseMessage;

int notificationStatus;
char *notificationMessage;

void clearMenu()
{
    int i;
    for(i = 1; i <= 30; i++)
    {
        gotoxy(1, i);
        printf("%-59s", " ");
    }
}

void printNotificationMessage()
{
    if(notificationMessage)
    {
        gotoxy(1, 20);
        printf(GRN);
        if(notificationStatus != 0)
        {
            printf(RED);
        }
        printf("%-59s", notificationMessage);
        printf(RESET);
    }
    notificationStatus = 0;
    notificationMessage = NULL;
}

void showRegisterMenu()
{
    clearMenu();

    gotoxy(1, 1);
    printf("%-59s", " ");
    gotoxy(1, 2);
    printf("%-59s", " ");
    gotoxy(1, 3);
    printf("%-59s", " ");
    gotoxy(1, 4);
    printf("%-59s", "REGISTER");
    gotoxy(1, 6);
    printf("%-59s", "    username:");
    gotoxy(15, 6);

    char username[256];
    if(scanf("%s", username) != 1)
    {
        while(getchar() != '\n');
    }

    gotoxy(1, 7);
    printf("%-59s", "    password:");
    gotoxy(15, 7);

    char password[256];
    if(scanf("%s", password) != 1)
    {
        while(getchar() != '\n');
    }

    gotoxy(1, 8);
    printf("%-59s", "    color:");
    gotoxy(15, 8);

    char color[256];
    if(scanf("%s", color) != 1)
    {
        while(getchar() != '\n');
    }

    gotoxy(1, 9);
    printf("%-59s", "    symbol:");
    gotoxy(15, 9);

    char symbol[256];
    if(scanf("%s", symbol) != 1)
    {
        while(getchar() != '\n');
    }

    sendRegisterRequest(username, password, color, symbol);
    while(registerResponseReceived == 0);
    registerResponseReceived = 0;

    notificationMessage = registerResponseMessage;
    notificationStatus = registerResponseStatus;
}

void showLoginMenu()
{
    clearMenu();

    gotoxy(1, 1);
    printf("%-59s", " ");
    gotoxy(1, 2);
    printf("%-59s", " ");
    gotoxy(1, 3);
    printf("%-59s", " ");
    gotoxy(1, 4);
    printf("%-59s", "LOGIN");
    gotoxy(1, 6);
    printf("%-59s", "    username:");

    gotoxy(15, 6);

    char username[256];
    if(scanf("%s", username) != 1)
    {
        while(getchar() != '\n');
    }

    gotoxy(1, 7);
    printf("%-59s", "    password:");
    gotoxy(15, 7);

    char password[256];
    if(scanf("%s", password) != 1)
    {
        while(getchar() != '\n');
    }
    sendLoginRequest(username, password);
    while(loginResponseReceived == 0);
    loginResponseReceived = 0;

    notificationMessage = loginResponseMessage;
    notificationStatus = loginResponseStatus;
}

void showGameMenu()
{
    clearMenu();

    gotoxy(1, 1);
    printf("%-59s", " ");
    gotoxy(1, 2);
    printf("%-59s", " ");
    gotoxy(1, 3);
    printf("%-59s", " ");
    gotoxy(1, 4);
    printf("%-59s", "GAME");
    gotoxy(1, 6);

}

void showMainMenu()
{
    int choice;
    int exit = 0;
    int invalid = 0;
    do
    {
        clearMenu();
        gotoxy(1, 1);
        printf("%-59s", " ");
        gotoxy(1, 2);
        printf("%-59s", " ");
        gotoxy(1, 3);
        printf("%-59s", " ");
        gotoxy(1, 4);
        printf("%-59s", "MAIN MENU");

        if(loginResponseStatus == 0)
        {
            gotoxy(1, 6);
            printf("%-59s", "    1 - Register");
            gotoxy(1, 8);
            printf("%-59s", "    2 - Login");
            gotoxy(1, 10);
            printf("%-59s", "    9 - Exit");
        }
        else{
            gotoxy(1, 6);
            printf("%-59s", "    1 - Move Up");
            gotoxy(1, 7);
            printf("%-59s", "    2 - Move Right");
            gotoxy(1, 8);
            printf("%-59s", "    3 - Move Down");
            gotoxy(1, 9);
            printf("%-59s", "    4 - Move Left");

            gotoxy(1, 11);
            printf("%-59s", "    9 - Logout");
        }

        gotoxy(1, 13);
        if(invalid)
        {
            printf(RED);
            printf("%-59s", "Invalid option!!");
            printf(RESET);
        }
        else
        {
            printf("%-59s", " ");
        }

        printNotificationMessage();

        gotoxy(1, 12);
        printf("%-59s", "Please make a choice: ");
        gotoxy(23, 12);
        if(scanf("%d", &choice) != 1)
        {
            choice = -1;
            while(getchar() != '\n');
        }

        invalid = 0;

        switch(choice)
        {
            case 1: {
                showRegisterMenu();
                break;
                }

            case 2: {
                showLoginMenu();
                break;
                }

            case 9:
                exit = 1;
                break;

            default:
                invalid = 1;
        }
    } while(!exit);
}

void setRegisterResponseReceived(int status, char *message)
{
    registerResponseStatus = status;
    registerResponseMessage = message;
    registerResponseReceived = 1;
}

void setLoginResponseReceived(int status, char *message)
{
    loginResponseStatus = status;
    loginResponseMessage = message;
    loginResponseReceived = 1;
}

void *menuThreadFunc(void *vargp)
{
    drawClientTitle();
    showMainMenu();
    exit(1);
}
