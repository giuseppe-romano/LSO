#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "../../include/protocol.h"
#include "draw.h"
#include "serial.h"
#include "logging.h"

#define MOVE_UP 10
#define MOVE_RIGHT 11
#define MOVE_DOWN 12
#define MOVE_LEFT 13

Cell *currentPlayerCell;
int registerResponseReceived = 0;
int registerResponseStatus = -1;
char *registerResponseMessage;

int loginResponseReceived = 0;
int loginResponseStatus = -1;
char *loginResponseMessage;
char loggedInUsername[200];

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

void showGameMenu()
{
    infoMenu("Show game menu");

    int choice;
    int exit = 0;
    int invalid = 0;
    do {
        clearMenu();

        gotoxy(1, 1);
        printf("%-59s", " ");
        gotoxy(1, 2);
        printf("%-59s", " ");
        gotoxy(1, 3);
        printf("%-59s", " ");

        gotoxy(1, 4);
        printf("%-59s", "PLAY GAME MENU");
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

        printNotificationMessage(notificationStatus, notificationMessage);
        notificationStatus = 0;
        notificationMessage = NULL;

        drawConnectedPlayer(currentPlayerCell);

        gotoxy(1, 12);
        printf("%-59s", "Please make a choice: ");
        gotoxy(23, 12);
        setInteractiveCursorCoords(23, 12);

        if(scanf("%d", &choice) != 1)
        {
            choice = -1;
            while(getchar() != '\n');
        }

        invalid = 0;

        switch(choice)
        {
            case 1: {
                sendMovePlayerRequest(currentPlayerCell, MOVE_UP);
                break;
                }

            case 2: {
                sendMovePlayerRequest(currentPlayerCell, MOVE_RIGHT);
                break;
                }

            case 3: {
                sendMovePlayerRequest(currentPlayerCell, MOVE_DOWN);
                break;
                }

            case 4: {
                sendMovePlayerRequest(currentPlayerCell, MOVE_LEFT);
                break;
                }

            case 9:
                sendLogoutRequest(currentPlayerCell->user);
                system("@cls||clear");
                drawClientTitle();
                drawConnectedPlayer(NULL);
                exit = 1;
                break;

            default:
                invalid = 1;
        }
    } while(!exit);
}


void showRegisterMenu()
{
    infoMenu("Show register menu");

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
    infoMenu("Show login menu");

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

    strcpy(loggedInUsername, username);

    sendLoginRequest(username, password);
    while(loginResponseReceived == 0);
    loginResponseReceived = 0;

    notificationMessage = loginResponseMessage;
    notificationStatus = loginResponseStatus;

    if(notificationStatus == 0)
    {
        strcpy(loggedInUsername, username);

        showGameMenu();
    }
    else
    {
        memset(loggedInUsername, '\0', 200);
    }
}

void showMainMenu()
{
    infoMenu("Show main menu");

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
        gotoxy(1, 6);
        printf("%-59s", "    1 - Register");
        gotoxy(1, 8);
        printf("%-59s", "    2 - Login");
        gotoxy(1, 10);
        printf("%-59s", "    9 - Exit");

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

        printNotificationMessage(notificationStatus, notificationMessage);
        notificationStatus = 0;
        notificationMessage = NULL;

        gotoxy(1, 12);
        printf("%-59s", "Please make a choice: ");
        gotoxy(23, 12);
        setInteractiveCursorCoords(23, 12);

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

void setCurrentPlayerCell(Cell *playerCell)
{
    if(strcmp(playerCell->user, loggedInUsername) == 0)
    {
        infoMenu("Setting player as current player");
        if(currentPlayerCell == NULL)
        {
            currentPlayerCell = (Cell*)malloc(sizeof(Cell));
        }
        currentPlayerCell->user = playerCell->user;
        currentPlayerCell->x = playerCell->x;
        currentPlayerCell->y = playerCell->y;
        currentPlayerCell->symbol = playerCell->symbol;
        currentPlayerCell->color = playerCell->color;
    }
}

void *menuThreadFunc(void *vargp)
{
    drawClientTitle();
    showMainMenu();
    exit(1);
}
