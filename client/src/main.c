#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

#include "draw.h"
#include "serial.h"
#include "logging.h"
#include "../include/menu.h"
#include "../include/protocol.h"

int main(){
    initLogFile("client.log");

    pthread_t menu_thread_id;

    char server_message[2000];
    int serverSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    // Create the socket.
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    //Configure settings of the server address
    // Address family is Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function
    serverAddr.sin_port = htons(7799);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
    //Connect the socket to the server using the address
    addr_size = sizeof(serverAddr);

    int serverConnected = connect(serverSocket, (struct sockaddr *) &serverAddr, addr_size);
    if(serverConnected != 0)
    {
        error("Unable to connect to the server");
        printf("Unable to connect to the server\n");
    }
    else {
        system("@cls||clear");
        info("Connected to the server!");

        //Creates a thread responsible for the menu console.
        pthread_create(&menu_thread_id, NULL, menuThreadFunc, NULL);

        setServerSocket(serverSocket);

        Game *currentGame = NULL;
        int serverAlive = 1;
        while(serverAlive)
        {
            memset(server_message, '\0', 2000);
            //Read the message from the server into the buffer
            if(recv(serverSocket, server_message, 2000, 0) <= 0)
            {
                warn("Connection closed. The server cut off!");
                printNotificationMessage(-1, "Connection closed. The server cut off!\n");
                serverAlive = 0;
            }
            else
            {
                AuthenticationResponse *authenticationResponse = NULL;
                MovePlayerResponse *movePlayerResponse = NULL;
                Game *game = NULL;
                Cell *cell = NULL;

                char logMessage[2100];
                sprintf(logMessage, "Complete message from server: '%s'", server_message);
                info(logMessage);

                //The server message can contain multiple messages, so it will be splitted
                char *buffer = strdup(server_message);
                char *message;
                while ((message = strsep(&buffer, "\n")) != NULL)
                {
                    if(strlen(message) > 0)
                    {
                        sprintf(logMessage, "Processing server message : '%s'", message);
                        info(logMessage);
                    }

                    //The server sent a register response
                    if((authenticationResponse = deserializeRegisterResponse(message)) != NULL)
                    {
                        sprintf(logMessage, "RegisterResponse: %s", message);
                        info(logMessage);

                        setRegisterResponseReceived(authenticationResponse->status, authenticationResponse->message);

                        free(authenticationResponse);
                        authenticationResponse = NULL;
                    }
                    //The server sent a login response
                    else if((authenticationResponse = deserializeLoginResponse(message)) != NULL)
                    {
                        sprintf(logMessage, "LoginResponse: %s", message);
                        info(logMessage);

                        setLoginResponseReceived(authenticationResponse->status, authenticationResponse->message);

                        free(authenticationResponse);
                        authenticationResponse = NULL;
                    }
                    //The server sent a move player response
                    else if((movePlayerResponse = deserializeMovePlayerResponse(message)) != NULL)
                    {
                        sprintf(logMessage, "MovePlayerResponse: %s", message);
                        info(logMessage);

                        Cell *current = currentGame->playerCells;
                        while (current != NULL) {
                            if(strcmp(current->user, movePlayerResponse->player->user) == 0) {
                                current->x = movePlayerResponse->player->x;
                                current->y = movePlayerResponse->player->y;
                            }

                            current = current->next;
                        }

                        switch(movePlayerResponse->status)
                        {
                            case ERR_PLAYER_MOVED_SUCCESS: {
                                sprintf(logMessage, "The player '%s' moved with success!", movePlayerResponse->player->user);
                                info(logMessage);
                                printNotificationMessage(ERR_PLAYER_MOVED_SUCCESS, logMessage);
                                break;
                            }
                            case ERR_PLAYER_HIT_BOMB: {
                                sprintf(logMessage, "The player '%s' hit a bomb. He blew up!", movePlayerResponse->player->user);
                                info(logMessage);
                                printNotificationMessage(ERR_PLAYER_HIT_BOMB, logMessage);
                                break;
                            }
                            case ERR_USER_WIN_GAME: {
                                sprintf(logMessage, "The player '%s' won!", movePlayerResponse->player->user);
                                info(logMessage);
                                printNotificationMessage(0, logMessage);
                                break;
                            }
                            default: {
                                //No action
                            }
                        }
                        drawMineField(currentGame);
                        setCursorToOffset();

                        free(movePlayerResponse);
                        movePlayerResponse = NULL;
                    }
                    //The server sent a new game
                    else if((game = deserializeGame(message)) != NULL)
                    {
                        sprintf(logMessage, "Game: %s", message);
                        info(logMessage);

                        if(currentGame != NULL)
                        {
                            free(currentGame);
                            currentGame = NULL;
                        }

                        currentGame = game;
                        currentGame->bombCells = NULL;
                        drawMineField(game);
                    }
                    //The server sent an added player
                    else if((cell = deserializeAddedCell(message)) != NULL)
                    {
                        sprintf(logMessage, "Added Player: %s", message);
                        info(logMessage);

                        if(currentGame)
                        {
                            info("Adding the new player to the game...");
                            if(currentGame->playerCells == NULL)
                            {
                                currentGame->playerCells = cell;
                            }
                            else
                            {
                                Cell *parent = currentGame->playerCells;
                                Cell *current = currentGame->playerCells;
                                while (current != NULL) {
                                    parent = current;
                                    current = current->next;
                                }
                                parent->next = cell;
                            }
                            sprintf(logMessage, "Player '%s' added to the game!", cell->user);
                            printNotificationMessage(0, logMessage);
                            info(logMessage);

                            info("Drawing the game...");
                            drawMineField(currentGame);
                            info("Game drove!");
                        }

                        setCurrentPlayerCell(cell);
                    }
                    //The server sent a removed player
                    else if((cell = deserializeRemovedCell(message)) != NULL)
                    {
                        sprintf(logMessage, "Removed Player: %s", message);
                        info(logMessage);

                        if(currentGame)
                        {
                            Cell *tmp = NULL;
                            Cell *parent = currentGame->playerCells;
                            if(strcmp(parent->user, cell->user) == 0) {
                                tmp = currentGame->playerCells;
                                currentGame->playerCells = parent->next;
                            }
                            else
                            {
                                Cell *current = currentGame->playerCells;
                                while (current != NULL) {
                                    if(strcmp(current->user, cell->user) == 0) {
                                        tmp = current;
                                    }

                                    current = current->next;
                                }
                            }

                            sprintf(logMessage, "Player '%s' removed from the game!", tmp->user);
                            printNotificationMessage(0, logMessage);
                            if(tmp != NULL)
                            {
                                free(tmp);
                            }
                        }
                        drawMineField(currentGame);

                        free(cell);
                        cell = NULL;
                    }
                }
            }
        }
    }

    pthread_cancel(menu_thread_id);
    close(serverSocket);

    return 0;
}
