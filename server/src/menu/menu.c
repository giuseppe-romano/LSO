#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "draw.h"
#include "../../include/game.h"
#include "../../include/player.h"

void showListPlayersMenu()
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
        printf("%-59s", "LIST PLAYERS MENU");
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

            case 2:
                showListPlayersMenu();
                break;

            case 9:
                exit = 1;
                break;

            default:
                invalid = 1;
        }
    } while(!exit);
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
        printf("%-59s", "    1 - Start/Resume Game");
        gotoxy(1, 8);
        printf("%-59s", "    2 - List connected players");
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
                Game *game = generateNewGame();
                broadcastNewGame(game);

                drawMineField(game);
                break;
                }

            case 2:
                showListPlayersMenu();
                break;

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
    drawServerTitle();
    showMainMenu();
    exit(1);
}
