#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "draw.h"
#include "../../include/game.h"
#include "../../include/player.h"

int currentXInputOffset = 0;
int currentYInputOffset = 0;

void clearMenu()
{
    int i;
    for(i = 1; i <= 30; i++)
    {
        gotoxy(1, i);
        printf("%-59s", " ");
    }
}

void setCursorToOffset()
{
    gotoxy(currentXInputOffset, currentYInputOffset);
}

void showPlayersMenu(char *category, Player *players)
{
    clearMenu();
    gotoxy(1, 1);
    printf("%-59s", " ");
    gotoxy(1, 2);
    printf("%-59s", " ");
    gotoxy(1, 3);
    printf("%-59s", " ");
    gotoxy(1, 4);

    char buffer[2000];
    sprintf(buffer, "%s players", category);
    printf("%-59s", buffer);

    if(players == NULL)
    {
        gotoxy(1, 6);
        sprintf(buffer, "    No %s players yet!", category);
        printf("%-59s", buffer);

        gotoxy(1, 8);
        printf("%-59s", "Press any key:");
        gotoxy(16, 8);
        currentXInputOffset = 16;
        currentYInputOffset = 8;
    }
    else
    {
        int offset = 6;
        Player *current = players;
        while (current != NULL) {
            gotoxy(1, offset);

            drawPlayer(current);

            offset += 2;
            current = current->next;
        }
        gotoxy(1, offset + 2);
        printf("%-59s", "Press any key:");
        gotoxy(16, offset + 2);
        currentXInputOffset = 16;
        currentYInputOffset = offset + 2;
    }

    int choice;
    if(scanf("%d", &choice) != 1)
    {
        while(getchar() != '\n');
    }
}

void showListPlayersMenu()
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
        currentXInputOffset = 23;
        currentYInputOffset = 12;

        if(scanf("%d", &choice) != 1)
        {
            choice = -1;
            while(getchar() != '\n');
        }

        invalid = 0;

        switch(choice)
        {
            case 1: {
                showPlayersMenu("registered", getRegisteredPlayers());
                break;
            }

            case 2:
                showPlayersMenu("connected", getConnectedPlayers());
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
        printf("%-59s", "    1 - Start a new Game");
        gotoxy(1, 8);
        printf("%-59s", "    2 - List players");
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
        currentXInputOffset = 23;
        currentYInputOffset = 12;
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
    //Creates a new game when boots
    Game *game = generateNewGame();
    drawMineField(game);

    drawServerTitle();
    showMainMenu();
    exit(1);
}
