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
#include "../../include/player.h"
#include "../../include/game.h"
#include "../../include/protocol.h"

int clientSockets[100];
int numSockets = 0;

char *registeredPlayersFilepath;
Player *registeredPlayers = NULL;
Player *connectedPlayers = NULL;

void addClientSocket(int clientSocket)
{
    clientSockets[numSockets++] = clientSocket;
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
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendGame(clientSockets[j], game);
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
        sprintf(logMessage, "Writing player '%s'", line);
        info(logMessage);

        line = serializePlayer(current);
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

pthread_mutex_t registerPlayerMutex = PTHREAD_MUTEX_INITIALIZER;
int registerPlayer(AuthenticationRequest *authenticationRequest)
{
    int status = 1;
    pthread_mutex_lock(&registerPlayerMutex);

    char logMessage[2000];
    sprintf(logMessage, "Registering player '%s'", authenticationRequest->username);
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

    //If there is a registered player having that specific username and the password matches, then he is logged in
    Player *registeredPlayer = getRegisteredPlayerByUsername(authenticationRequest->username);
    if(registeredPlayer != NULL && (strcmp(registeredPlayer->password, authenticationRequest->password) == 0))
    {
        Player *player;
        memcpy(&registeredPlayer, &player, sizeof(Player));

        Player *parent = connectedPlayers;
        Player *current = connectedPlayers;
        while(current != NULL)
        {
            parent = current;
            current = current->next;
        }

        parent->next = player;

        status = 0;
    }

    pthread_mutex_unlock(&loginPlayerMutex);
    return status;
}


void broadcastPlayerMoved(Cell *player, int status)
{
    int j;
    for( j = 0 ; j < numSockets; j++ ) {
        sendMovePlayerResponse(clientSockets[j], player, status);
    }
}

pthread_mutex_t authenticationMutex = PTHREAD_MUTEX_INITIALIZER;
void *playerThreadFunc(void *vargp)
{
    int clientSocket = *((int *)vargp);

    addClientSocket(clientSocket);
    char client_message[2000];

    int clientConnected = 1;
    while(clientConnected)
    {
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
                sendLoginResponse(clientSocket, status, message);

                //If login correct then the player joins the current game (if any)
                if(status == 0)
                {
                    Game *game = getCurrentGame();
                    if(game != NULL)
                    {
                        Cell *player = (Cell*) malloc(sizeof(Cell));
                        player->user = authenticationRequest->username;
                        player->x = 0;
                        player->y = 12;
                        player->symbol = "G";
                        player->color = "\x1B[35m";

                        addPlayer(game, player);
                        sendGame(clientSocket, game);
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
                int index = indexOfPlayer(game, movePlayerRequest->player);

                broadcastPlayerMoved(&(game->playerCells[index]), status);
            }
        }
    }

    removeClientSocket(clientSocket);
    close(clientSocket);
    pthread_exit(NULL);
}
