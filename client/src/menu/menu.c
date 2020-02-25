#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "draw.h"

void clearMenu()
{
    int i;
    for(i = 1; i <= 30; i++)
    {
        gotoxy(1, i);
        printf("%-59s", " ");
    }
}

void showSignUpMenu()
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
        printf("%-59s", "SIGN IN");
        gotoxy(1, 6);
        printf("%-59s", "    1 - List registered players");
        gotoxy(1, 8);
        printf("%-59s", "    2 - List connected players");
        gotoxy(1, 10);
        printf("%-59s", "    9 - Back to MAIN MENU");

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
                //Game *game = generateNewGame();
                //drawScreen(game);

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

void showSignInMenu()
{
    clearMenu();

    gotoxy(1, 1);
    printf("%-59s", " ");
    gotoxy(1, 2);
    printf("%-59s", " ");
    gotoxy(1, 3);
    printf("%-59s", " ");
    gotoxy(1, 4);
    printf("%-59s", "SIGN IN");
    gotoxy(1, 6);
    printf("%-59s", "    username:");

    gotoxy(14, 6);

    char username[256];
    if(scanf("%s", &username) != 1)
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
    printf("%-59s", "SIGN IN");
    gotoxy(1, 6);
    printf("%-59s", "    password:");

    gotoxy(14, 6);
    char password[256];
    if(scanf("%s", &password) != 1)
    {
        while(getchar() != '\n');
    }
    printf("user is %s", username);
    printf("pwd is %s", password);
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
        gotoxy(1, 6);
        printf("%-59s", "    1 - Sign up");
        gotoxy(1, 8);
        printf("%-59s", "    2 - Sign in");
        gotoxy(1, 10);
        printf("%-59s", "    9 - Exit");

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
                showSignUpMenu();
                break;
                }

            case 2: {
                showSignInMenu();
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

void *menuThreadFunc(void *vargp)
{
    drawClientTitle();
    showMainMenu();
    exit(1);
}
