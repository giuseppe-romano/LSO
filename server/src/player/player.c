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
    clientSockets[numSockets++] = clientSocket;
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
    info("Broadcasting new game...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendGame(clientSockets[j], game);
    }
}

void broadcastAddedPlayer(Cell *player)
{
    info("Broadcasting new player...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendAddedCell(clientSockets[j], player);
    }
}

void broadcastRemovedPlayer(Cell *player)
{
    info("Broadcasting removed player...");
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendRemovedCell(clientSockets[j], player);
    }
}

void broadcastPlayerMoved(Cell *player, int status)
{
    info("Broadcasting player moved...");
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
    info(logMessage);

    registeredPlayersFile = fopen(filepath, "r");
    if (registeredPlayersFile == NULL)
    {
        sprintf(logMessage, "The database file '%s' does not exist!", filepath);
        warn(logMessage);
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
        info(logMessage);

        Player *p = deserializePlayer(line);
        if(head == NULL)
        {
            head = p;
            current = head;
        }
        else
        {
            current->next = p;
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
    info(logMessage);

    registeredPlayersFile = fopen(registeredPlayersFilepath, "w");

    Player *current = registeredPlayers;
    while (current != NULL) {
        line = serializePlayer(current);

        sprintf(logMessage, "Writing player '%s'", line);
        info(logMessage);

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
    info(logMessage);

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
        warn(logMessage);

        writeRegisteredPlayers();

        status = 0;
    }
    else{
        sprintf(logMessage, "Player '%s' already exists!", authenticationRequest->username);
        warn(logMessage);
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
    info(logMessage);

    //If there is a registered player having that specific username and the password matches, then he is logged in
    Player *registeredPlayer = getRegisteredPlayerByUsername(authenticationRequest->username);
    if(registeredPlayer != NULL && (strcmp(registeredPlayer->password, authenticationRequest->password) == 0))
    {
        sprintf(logMessage, "Player '%s' is authenticated successfully", authenticationRequest->username);
        info(logMessage);

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

    pthread_mutex_unlock(&loginPlayerMutex);
    return status;
}

void logoutPlayer(AuthenticationRequest *authenticationRequest)
{
    pthread_mutex_lock(&loginPlayerMutex);

    char logMessage[2000];
    sprintf(logMessage, "Logging-out player '%s'", authenticationRequest->username);
    info(logMessage);

    Player *connectedPlayer = getConnectedPlayerByUsername(authenticationRequest->username);
    removeConnectedPlayer(connectedPlayer);

    pthread_mutex_unlock(&loginPlayerMutex);
}

pthread_mutex_t authenticationMutex = PTHREAD_MUTEX_INITIALIZER;
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
           warn("Connection closed. The client cut off!");
           clientConnected = 0;
        }
        else
        {
            AuthenticationRequest *authenticationRequest = NULL;
            MovePlayerRequest *movePlayerRequest = NULL;
            char logMessage[2100];

            sprintf(logMessage, "New message from client: %s", client_message);
            info(logMessage);

            //The client sent a register request
            if((authenticationRequest = deserializeRegisterRequest(client_message)) != NULL)
            {
                sprintf(logMessage, "RegisterAction: %s", client_message);
                info(logMessage);

                int status = registerPlayer(authenticationRequest);
                char *message = "Registration done!";
                if(status == 1)
                {
                    message = "Registration failed. Username already exists!";
                }
                sendRegisterResponse(clientSocket, status, message);
            }
            //The client sent a login request
            else if((authenticationRequest = deserializeLoginRequest(client_message)) != NULL)
            {
                sprintf(logMessage, "LoginAction: %s", client_message);
                info(logMessage);

                int status = loginPlayer(authenticationRequest);
                char *message = "Login succeeded!";
                if(status == 1)
                {
                    message = "Login failed. Username and/or password incorrect!";
                }
                sprintf(logMessage, "Login: %s", message);
                info(logMessage);

                sendLoginResponse(clientSocket, status, message);

                //If login correct then the player joins the current game (if any)
                if(status == 0)
                {
                    Game *game = getCurrentGame();
                    if(game != NULL)
                    {
                        Player *connectedPlayer = getConnectedPlayerByUsername(authenticationRequest->username);
                        Cell *playerCell = (Cell*) malloc(sizeof(Cell));
                        playerCell->user = authenticationRequest->username;
                        playerCell->x = 0;
                        playerCell->y = 12;
                        playerCell->symbol = connectedPlayer->symbol;
                        playerCell->color = connectedPlayer->color;

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
            }
            //The client sent a move request
            else if((movePlayerRequest = deserializeMovePlayerRequest(client_message)) != NULL)
            {
                char str[2100];
                sprintf(str, "MovePlayerAction: %s", client_message);
                info(str);

                Game *game = getCurrentGame();
                int status = movePlayer(game, movePlayerRequest->player, movePlayerRequest->direction);
                Cell *playerCell = getPlayerByUsername(game, movePlayerRequest->player->user);

                broadcastPlayerMoved(playerCell, status);
            }
            //The client sent a logout request
            else if((authenticationRequest = deserializeLogoutRequest(client_message)) != NULL)
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
            }
        }
    }

    removeClientSocket(clientSocket);
    close(clientSocket);

    removePlayer(getCurrentGame(), currentPlayerCell);
    removeConnectedPlayer(currentConnectedPlayer);
    broadcastRemovedPlayer(currentPlayerCell);
    drawMineField(getCurrentGame());
    setCursorToOffset();

    pthread_exit(NULL);
}
