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
    system("@cls||clear");
    initLogFile("client.log");

    //Creates a thread responsible for the menu console.
    pthread_t menu_thread_id;
    pthread_create(&menu_thread_id, NULL, menuThreadFunc, NULL);

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

    connect(serverSocket, (struct sockaddr *) &serverAddr, addr_size);
    info("Connected to the server!");

    setServerSocket(serverSocket);

    int serverConnected = 1;
    while(serverConnected)
    {
        memset(server_message, '\0', 2000);
        //Read the message from the server into the buffer
        if(recv(serverSocket, server_message, 2000, 0) <= 0)
        {
            warn("Connection closed. The server cut off!");
            serverConnected = 0;
        }
        else
        {
            AuthenticationResponse *authenticationResponse = NULL;
            MovePlayerResponse *movePlayerResponse = NULL;
            Game *game = NULL;

            char *message = strtok(server_message, "\n");

            while( message != NULL ) {

                //The server sent a register response
                if((authenticationResponse = deserializeRegisterResponse(server_message)) != NULL)
                {
                    char str[2100];
                    sprintf(str, "RegisterResponse: %s", server_message);
                    info(str);

                    setUserLoggedIn(authenticationResponse->status);
                }
                //The server sent a login response
                else if((authenticationResponse = deserializeLoginResponse(server_message)) != NULL)
                {
                    char str[2100];
                    sprintf(str, "LoginResponse: %s", server_message);
                    info(str);

                    setUserLoggedIn(authenticationResponse->status);
                }
                //The server sent a move player response
                else if((movePlayerResponse = deserializeMovePlayerResponse(server_message)) != NULL)
                {
                    char str[2100];
                    sprintf(str, "MovePlayerResponse: %s", server_message);
                    info(str);
                }
                //The server sent a new game
                else if((game = deserializeGame(server_message)) != NULL)
                {
                    char str[2100];
                    sprintf(str, "Game: %s", server_message);
                    info(str);

                    drawMineField(game);
                }

                message = strtok(NULL, "|");
            }

        }

    }
    close(serverSocket);

    pthread_join(menu_thread_id,NULL);

    return 0;
}
