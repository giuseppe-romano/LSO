#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <getopt.h>

#include "draw.h"
#include "serial.h"
#include "logging.h"
#include "../include/menu.h"
#include "../include/protocol.h"

static struct option long_options[] =
{
    {"host",  required_argument, 0, 'h'},
    {"port",  required_argument, 0, 'p'},
    {"log",   required_argument, 0, 'l'},
    {0, 0, 0, 0}
};

void printUsage()
{
    printf("Unable to start the Minefield client.\n");
    printf("Usage: ./client --options--\n");
    printf("\tOptions are:\n"
           "\t\t--host: (mandatory) server hostname or IP address\n"
           "\t\t--port: (mandatory) server listening port\n"
           "\t\t--log:  (optional)  the log filename (default: ./client.log)\n\n");
    printf("Example: ./client --host localhost --port 8000 --log ./client.log\n\n");
}

int main(int argc, char *argv[ ])
{
    int option_index = 0;

    char *host = "";
    char *logFile = "client.log";
    int port = -1;

    int c;
    while ((c = getopt_long(argc, argv, "h:p:l:", long_options, &option_index)) != -1) {
        switch (c)
        {
            case 'h':
              host = optarg;
              break;

            case 'p':
              sscanf(optarg, "%d", &port);
              break;

            case 'l':
              logFile = optarg;
              break;
        }
    }

    //If no hostname or port number specified than aborts printing the usage message.
    if(strlen(host) <= 0 || port == -1)
    {
        printUsage();
        exit(-1);
    }

    //Initializes the log file on which any activity will be logged
    initLogFile(logFile);

    char logMessage[2100];
    infoMain("Starting Minefield client...");
    sprintf(logMessage, "Connecting to the server: '%s:%d'", host, port);
    infoMain(logMessage);

    pthread_t menu_thread_id;

    char server_message[2000];
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Create the socket.
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    //Configure settings of the server address
    // Address family is Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function
    serverAddr.sin_port = htons(port);
    //Set IP address to localhost
    struct hostent *hp = gethostbyname(host);
    if(hp == NULL)
    {
        sprintf(logMessage, "Unable to resolve the server hostname: '%s'\n\n", host);
        errorMain(logMessage);
        printf("%s", logMessage);
        exit(-1);
    }
    memcpy(&(serverAddr.sin_addr),  hp->h_addr, hp->h_length);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    //Connect the socket to the server using the address
    int serverConnected = connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(serverConnected != 0)
    {
        sprintf(logMessage, "Unable to connect to the server: '%s:%d'. Please check your parameters!\n\n", host, port);
        errorMain(logMessage);
        printf("%s\n", logMessage);
        perror("Unable to connect");
        exit(-1);
    }
    else {
        system("@cls||clear");
        infoMain("Connected to the server!");

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
                warnMain("Connection closed. The server cut off!");
                printNotificationMessage(-1, "Connection closed. The server cut off!\n");
                serverAlive = 0;
            }
            else
            {
                AuthenticationResponse *authenticationResponse = NULL;
                MovePlayerResponse *movePlayerResponse = NULL;
                Game *game = NULL;
                Cell *cell = NULL;

                sprintf(logMessage, "Complete message from server: '%s'", server_message);
                infoMain(logMessage);

                //The server message can contain multiple messages, so it will be splitted
                char *buffer = strdup(server_message);
                char *message;
                while ((message = strsep(&buffer, "\n")) != NULL)
                {
                    if(strlen(message) > 0)
                    {
                        sprintf(logMessage, "Processing server message : '%s'", message);
                        infoMain(logMessage);
                    }

                    //The server sent a register response
                    if((authenticationResponse = deserializeRegisterResponse(message)) != NULL)
                    {
                        sprintf(logMessage, "RegisterResponse: %s", message);
                        infoMain(logMessage);

                        setRegisterResponseReceived(authenticationResponse->status, authenticationResponse->message);

                        free(authenticationResponse);
                        authenticationResponse = NULL;
                    }
                    //The server sent a login response
                    else if((authenticationResponse = deserializeLoginResponse(message)) != NULL)
                    {
                        sprintf(logMessage, "LoginResponse: %s", message);
                        infoMain(logMessage);

                        setLoginResponseReceived(authenticationResponse->status, authenticationResponse->message);

                        free(authenticationResponse);
                        authenticationResponse = NULL;
                    }
                    //The server sent a move player response
                    else if((movePlayerResponse = deserializeMovePlayerResponse(message)) != NULL)
                    {
                        sprintf(logMessage, "MovePlayerResponse: %s", message);
                        infoMain(logMessage);

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
                                sprintf(logMessage, "The player '%s' (%s) moved with success!", movePlayerResponse->player->symbol, movePlayerResponse->player->user);
                                infoMain(logMessage);
                                printNotificationMessage(ERR_PLAYER_MOVED_SUCCESS, logMessage);
                                break;
                            }
                            case ERR_PLAYER_HIT_BOMB: {
                                sprintf(logMessage, "The player '%s' (%s) hit a bomb. He blew up!", movePlayerResponse->player->symbol, movePlayerResponse->player->user);
                                warnMain(logMessage);
                                printNotificationMessage(ERR_PLAYER_HIT_BOMB, logMessage);
                                break;
                            }
                            case ERR_USER_WIN_GAME: {
                                sprintf(logMessage, "The player '%s' (%s) won! Game restarted!", movePlayerResponse->player->symbol, movePlayerResponse->player->user);
                                infoMain(logMessage);
                                printNotificationMessage(0, logMessage);
                                break;
                            }
                        }
                        drawMineField(currentGame);

                        free(movePlayerResponse);
                        movePlayerResponse = NULL;
                    }
                    //The server sent a new game
                    else if((game = deserializeGame(message)) != NULL)
                    {
                        sprintf(logMessage, "Game: %s", message);
                        infoMain(logMessage);

                        if(currentGame != NULL)
                        {
                            free(currentGame);
                            currentGame = NULL;
                        }

                        currentGame = game;
                        drawMineField(game);
                    }
                    //The server sent an added player
                    else if((cell = deserializeAddedCell(message)) != NULL)
                    {
                        sprintf(logMessage, "Added Player: %s", message);
                        infoMain(logMessage);

                        if(currentGame)
                        {
                            infoMain("Adding the new player to the game...");
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
                            sprintf(logMessage, "Player '%s' (%s) added to the game!", cell->symbol, cell->user);
                            printNotificationMessage(0, logMessage);
                            infoMain(logMessage);

                            infoMain("Drawing the game...");
                            drawMineField(currentGame);
                            infoMain("Game drove!");
                        }

                        setCurrentPlayerCell(cell);
                    }
                    //The server sent a removed player
                    else if((cell = deserializeRemovedCell(message)) != NULL)
                    {
                        sprintf(logMessage, "Removed Player: %s", message);
                        infoMain(logMessage);

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

                            sprintf(logMessage, "Player '%s' (%s) logged out!", tmp->symbol, tmp->user);
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
                    setInteractiveCursorCoords(23, 12);
                    gotoxy(23, 12);
                }
            }
        }
    }

    pthread_cancel(menu_thread_id);
    close(serverSocket);

    return 0;
}
