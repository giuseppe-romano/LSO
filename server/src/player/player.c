#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include "serial.h"
#include "logging.h"
#include "draw.h"
#include "../../include/player.h"
#include "../../include/game.h"
#include "../../include/protocol.h"
#include "../../include/menu.h"

int clientSockets[100];
int numSockets = 0;

char *registeredPlayersFilepath;
Player *registeredPlayers = NULL;
Player *connectedPlayers = NULL;

void addClientSocket(int clientSocket)
{
    char logMessage[200];
    sprintf(logMessage, "Adding client socket '%d' into the list", clientSocket);
    infoPlayer(logMessage);

    clientSockets[numSockets++] = clientSocket;
}

void shutdownClientSockets()
{
    infoPlayer("Shutting down all client's sockets...");
    char logMessage[200];

    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sprintf(logMessage, "Shutting down client's socket %d...", clientSockets[j]);
        infoPlayer(logMessage);

        shutdown(clientSockets[j], SHUT_RDWR);
    }
}

pthread_t playerThreads[100];
int numThreads = 0;
void addPlayerThread(pthread_t playerThread)
{
    infoPlayer("Adding a new player's thread into the list");

    playerThreads[numThreads++] = playerThread;
}

void killPlayerThreads()
{
    infoPlayer("Killing all player's threads...");

    int j;
    for( j = 0 ; j < numThreads; j++ ) {
        pthread_cancel(playerThreads[j]);
    }
}

Player* getRegisteredPlayers()
{
    return registeredPlayers;
}

Player* getConnectedPlayers()
{
    return connectedPlayers;
}

void removeConnectedPlayer(Player *player)
{
    if(connectedPlayers == NULL)
    {
        return;
    }
    if (strcmp(connectedPlayers->username, player->username) == 0)
    {
        connectedPlayers = connectedPlayers->next;
    }
    else
    {
        Player *parent = connectedPlayers;
        Player *current = connectedPlayers;
        while (current != NULL) {
            if (strcmp(current->username, player->username) == 0)
            {
                parent->next = current->next;
            }
            parent = current;
            current = current->next;
        }
    }
}

void removeClientSocket(int clientSocket)
{
    int j;
    int counter = 0;
    int n = numSockets;
    for( j = 0 ; j < n; j++ ) {
        if(clientSockets[j] != clientSocket) {
            clientSockets[counter++] = clientSockets[j];
        }
        else {
            numSockets--;
        }
    }
}

void broadcastNewGame(Game *game)
{
    infoPlayer("Broadcasting new game...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendGame(clientSockets[j], game);
    }
}

void broadcastAddedPlayer(Cell *player)
{
    infoPlayer("Broadcasting new player...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendAddedCell(clientSockets[j], player);
    }
}

void broadcastRemovedPlayer(Cell *player)
{
    infoPlayer("Broadcasting removed player...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendRemovedCell(clientSockets[j], player);
    }
}

void broadcastPlayerMoved(Cell *player, int status)
{
    infoPlayer("Broadcasting player moved...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendMovePlayerResponse(clientSockets[j], player, status);
    }
}

void loadRegisteredPlayers(char *filepath)
{
    registeredPlayersFilepath = filepath;
    FILE * registeredPlayersFile;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    char logMessage[2000];
    sprintf(logMessage, "Loading registered players from database file '%s'", filepath);
    infoPlayer(logMessage);

    registeredPlayersFile = fopen(filepath, "r");
    if (registeredPlayersFile == NULL)
    {
        sprintf(logMessage, "The database file '%s' does not exist!", filepath);
        warnPlayer(logMessage);
        return;
    }

    Player *head = NULL;
    Player *current = NULL;
    while ((read = getline(&line, &len, registeredPlayersFile)) != -1) {
        if(line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = 0;
        }

        sprintf(logMessage, "Loaded player '%s'", line);
        infoPlayer(logMessage);

        Player *p = deserializePlayer(line);
        if(head == NULL)
        {
            head = p;
            current = head;
        }
        else
        {
            current->next = p;
            current = current->next;
        }
    }

    fclose(registeredPlayersFile);
    if (line)
    {
        free(line);
    }

    registeredPlayers = head;
}

void writeRegisteredPlayers()
{
    FILE * registeredPlayersFile;
    char * line = NULL;

    char logMessage[2000];
    sprintf(logMessage, "Writing registered players to database file '%s'", registeredPlayersFilepath);
    infoPlayer(logMessage);

    registeredPlayersFile = fopen(registeredPlayersFilepath, "w");

    Player *current = registeredPlayers;
    while (current != NULL) {
        line = serializePlayer(current);

        sprintf(logMessage, "Writing player '%s'", line);
        infoPlayer(logMessage);

        fprintf (registeredPlayersFile, "%s\n", line);
        if (line)
        {
            free(line);
        }

        current = current->next;
    }
    fclose(registeredPlayersFile);
}

Player* getRegisteredPlayerByUsername(char *username)
{
    Player *res = NULL;

    Player *current = registeredPlayers;
    while(current != NULL && res == NULL)
    {
        if(strcmp(current->username, username) == 0)
        {
            res = current;
        }
        current = current->next;
    }
    return res;
}

Player* getConnectedPlayerByUsername(char *username)
{
    Player *res = NULL;

    Player *current = connectedPlayers;
    while(current != NULL && res == NULL)
    {
        if(strcmp(current->username, username) == 0)
        {
            res = current;
        }
        current = current->next;
    }
    return res;
}

pthread_mutex_t registerPlayerMutex = PTHREAD_MUTEX_INITIALIZER;
int registerPlayer(AuthenticationRequest *authenticationRequest)
{
    int status = 1;
    pthread_mutex_lock(&registerPlayerMutex);

    char logMessage[2000];
    sprintf(logMessage, "Registering player '%s' with symbol '%s'", authenticationRequest->username, authenticationRequest->symbol);
    infoPlayer(logMessage);

    //If there is no registered player having that specific username
    if(getRegisteredPlayerByUsername(authenticationRequest->username) == NULL)
    {
        Player *player = (Player*) malloc(sizeof(Player));
        player->username = authenticationRequest->username;
        player->password = authenticationRequest->password;
        player->symbol = authenticationRequest->symbol;
        player->color = authenticationRequest->color;

        if(registeredPlayers == NULL)
        {
            registeredPlayers = player;
        }
        else{
            Player *parent = registeredPlayers;
            Player *current = registeredPlayers;
            while(current != NULL)
            {
                parent = current;
                current = current->next;
            }

            parent->next = player;
        }
        sprintf(logMessage, "Player '%s' registered with success!", authenticationRequest->username);
        warnPlayer(logMessage);

        writeRegisteredPlayers();

        status = 0;
    }
    else{
        sprintf(logMessage, "Player '%s' already exists!", authenticationRequest->username);
        warnPlayer(logMessage);
    }

    pthread_mutex_unlock(&registerPlayerMutex);
    return status;
}

pthread_mutex_t loginPlayerMutex = PTHREAD_MUTEX_INITIALIZER;
int loginPlayer(AuthenticationRequest *authenticationRequest)
{
    int status = 1;
    pthread_mutex_lock(&loginPlayerMutex);

    char logMessage[2000];
    sprintf(logMessage, "Logging-in player '%s'", authenticationRequest->username);
    infoPlayer(logMessage);

    //If there is a registered player having that specific username and the password matches, then he is logged in
    Player *registeredPlayer = getRegisteredPlayerByUsername(authenticationRequest->username);
    if(registeredPlayer != NULL && (strcmp(registeredPlayer->password, authenticationRequest->password) == 0))
    {
        Player *alreadyConnectedPlayer = getConnectedPlayerByUsername(authenticationRequest->username);
        if(alreadyConnectedPlayer != NULL)
        {
            status = 2; //Another user already logged in with same credentials, no way to login twice
        }
        else
        {
            sprintf(logMessage, "Player '%s' is authenticated successfully", authenticationRequest->username);
            infoPlayer(logMessage);

            Player *player = (Player*) malloc(sizeof(Player));
            player->username = registeredPlayer->username;
            player->password = registeredPlayer->password;
            player->symbol = registeredPlayer->symbol;
            player->color = registeredPlayer->color;

            if(connectedPlayers == NULL)
            {
                connectedPlayers = player;
            }
            else{
                Player *parent = connectedPlayers;
                Player *current = connectedPlayers;
                while(current != NULL)
                {
                    parent = current;
                    current = current->next;
                }
                parent->next = player;
            }

            status = 0;
        }
    }

    pthread_mutex_unlock(&loginPlayerMutex);
    return status;
}

void logoutPlayer(AuthenticationRequest *authenticationRequest)
{
    pthread_mutex_lock(&loginPlayerMutex);

    char logMessage[2000];
    sprintf(logMessage, "Logging-out player '%s'", authenticationRequest->username);
    infoPlayer(logMessage);

    Player *connectedPlayer = getConnectedPlayerByUsername(authenticationRequest->username);
    removeConnectedPlayer(connectedPlayer);

    pthread_mutex_unlock(&loginPlayerMutex);
}

void *playerThreadFunc(void *vargp)
{
    int clientSocket = *((int *)vargp);

    addClientSocket(clientSocket);
    char client_message[2000];

    Cell *currentPlayerCell = NULL;
    Player *currentConnectedPlayer = NULL;

    int clientConnected = 1;
    while(clientConnected)
    {
        memset(client_message, '\0', 2000);
        if(recv(clientSocket, client_message, 2000, 0) <= 0)
        {
           warnPlayer("Connection closed. The client cut off!");
           clientConnected = 0;
        }
        else
        {
            AuthenticationRequest *authenticationRequest = NULL;
            MovePlayerRequest *movePlayerRequest = NULL;
            char logMessage[2100];

            sprintf(logMessage, "Complete message from client: %s", client_message);
            infoPlayer(logMessage);

            //The server message can contain multiple messages, so it will be splitted
            char *buffer = strdup(client_message);
            char *message;
            while ((message = strsep(&buffer, "\n")) != NULL)
            {
                if(strlen(message) > 0)
                {
                    sprintf(logMessage, "Message from client: '%s'", message);
                    infoPlayer(logMessage);
                }

                //The client sent a register request
                if((authenticationRequest = deserializeRegisterRequest(message)) != NULL)
                {
                    sprintf(logMessage, "RegisterAction: %s", message);
                    infoPlayer(logMessage);

                    int status = registerPlayer(authenticationRequest);
                    char *resMessage = "Registration done!";
                    if(status == 1)
                    {
                        resMessage = "Registration failed. Username already exists!";
                    }
                    sendRegisterResponse(clientSocket, status, resMessage);

                    free(authenticationRequest);
                    authenticationRequest = NULL;
                }
                //The client sent a login request
                else if((authenticationRequest = deserializeLoginRequest(message)) != NULL)
                {
                    sprintf(logMessage, "LoginAction: %s", message);
                    infoPlayer(logMessage);

                    int status = loginPlayer(authenticationRequest);
                    char *resMessage = "Login succeeded!";
                    if(status == 1)
                    {
                        resMessage = "Login failed. Username and/or password incorrect!";
                    }
                    else if(status == 2)
                    {
                        resMessage = "Login failed. Another user logged in with same credentials!";
                    }
                    sprintf(logMessage, "Login: %s", resMessage);
                    infoPlayer(logMessage);

                    sendLoginResponse(clientSocket, status, resMessage);

                    //If login correct then the player joins the current game (if any)
                    if(status == 0)
                    {
                        Game *game = getCurrentGame();
                        if(game != NULL)
                        {
                            Player *connectedPlayer = getConnectedPlayerByUsername(authenticationRequest->username);

                            Cell *playerCell = generateRandomPlayerCell(connectedPlayer);

                            //Send first the game without the new player
                            sendGame(clientSocket, game);

                            //Then, add the player to the game
                            addPlayer(game, playerCell);

                            //And finally broadcasts to all connected players about the new player just joined
                            broadcastAddedPlayer(playerCell);

                            currentPlayerCell = playerCell;
                            currentConnectedPlayer = connectedPlayer;

                            drawMineField(game);
                            setCursorToOffset();
                        }
                    }
                    free(authenticationRequest);
                    authenticationRequest = NULL;
                }
                //The client sent a move request
                else if((movePlayerRequest = deserializeMovePlayerRequest(message)) != NULL)
                {
                    char str[2100];
                    sprintf(str, "MovePlayerAction: %s", message);
                    infoPlayer(str);

                    Game *game = getCurrentGame();
                    int status = movePlayer(game, movePlayerRequest->player, movePlayerRequest->direction);
                    Cell *playerCell = getPlayerByUsername(game, movePlayerRequest->player->user);

                    if(status == ERR_PLAYER_HIT_BOMB)
                    {
                        //The player blew up, removing
                        removePlayer(game, playerCell);

                        //Move back to the first column to retry
                        playerCell = generateRandomPlayerCell(currentConnectedPlayer);
                        addPlayer(game, playerCell);

                    }

                    if(status != ERR_CELL_BUSY)
                    {
                        broadcastPlayerMoved(playerCell, status);
                    }

                    //If the user won the game then a new game gets generated and notified to all connected players
                    if(status == ERR_USER_WIN_GAME)
                    {
                        game = generateNewGame();
                        broadcastNewGame(game);
                    }

                    drawMineField(game);
                    setCursorToOffset();

                    free(movePlayerRequest);
                    movePlayerRequest = NULL;
                }
                //The client sent a logout request
                else if((authenticationRequest = deserializeLogoutRequest(message)) != NULL)
                {
                    logoutPlayer(authenticationRequest);

                    Game *game = getCurrentGame();
                    //remove the player from the game
                    removePlayer(game, currentPlayerCell);

                    //And finally broadcasts to all connected players about the removed player
                    broadcastRemovedPlayer(currentPlayerCell);

                    drawMineField(game);
                    setCursorToOffset();

                    currentPlayerCell = NULL;
                    currentConnectedPlayer = NULL;

                    free(authenticationRequest);
                    authenticationRequest = NULL;
                }
            }
        }
    }

    removeClientSocket(clientSocket);
    close(clientSocket);

    if(currentPlayerCell != NULL)
    {
        removePlayer(getCurrentGame(), currentPlayerCell);
        broadcastRemovedPlayer(currentPlayerCell);
    }
    if(currentConnectedPlayer != NULL)
    {
        removeConnectedPlayer(currentConnectedPlayer);
    }
    drawMineField(getCurrentGame());
    setCursorToOffset();

    pthread_exit(NULL);
}
