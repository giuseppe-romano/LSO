#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "../../include/protocol.h"
#include "draw.h"

int loggedIn = 0;
int refresh = 0;

void clearMenu()
{
    int i;
    for(i = 1; i <= 30; i++)
    {
        gotoxy(1, i);
        printf("%-59s", " ");
    }
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

    gotoxy(14, 6);

    char username[256];
    if(scanf("%s", username) != 1)
    {
        while(getchar() != '\n');
    }

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
    printf("%-59s", "    password:");

    gotoxy(14, 6);
    char password[256];
    if(scanf("%s", password) != 1)
    {
        while(getchar() != '\n');
    }

    sendRegisterRequest(username, password);
    while(refresh == 0);
    refresh = 0;
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

    gotoxy(14, 6);

    char username[256];
    if(scanf("%s", username) != 1)
    {
        while(getchar() != '\n');
    }

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
    printf("%-59s", "    password:");

    gotoxy(14, 6);
    char password[256];
    if(scanf("%s", password) != 1)
    {
        while(getchar() != '\n');
    }

    sendLoginRequest(username, password);
    while(refresh == 0);
    refresh = 0;
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
        gotoxy(1, 1);
        printf("%-59s", " ");
        gotoxy(1, 2);
        printf("%-59s", " ");
        gotoxy(1, 3);
        printf("%-59s", " ");
        gotoxy(1, 4);
        printf("%-59s", "MAIN MENU");

        if(loggedIn == 0)
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
            printf("%-59s", "    9 - Exit");
        }

        gotoxy(1, 13);
        if(invalid)
        {
            printf("\033[31m");
            printf("%-59s", "Invalid option!!");
            printf("\033[0m");
        }
        else
        {
            printf("%-59s", " ");
        }

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

void setUserLoggedIn(int status)
{
    loggedIn = (status == 0) ? 1 : 0;
    refresh = 1;
}

void *menuThreadFunc(void *vargp)
{
    drawClientTitle();
    showMainMenu();
    exit(1);
}
