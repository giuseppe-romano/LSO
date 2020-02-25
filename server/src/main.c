#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

#include "draw.h"
#include "serial.h"
#include "logging.h"
#include "../include/game.h"
#include "../include/protocol.h"
#include "../include/menu.h"
#include "../include/player.h"

#define MAX 80
#define PORT 8080
#define SA struct sockaddr


//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/* S E R V E R */


int main()
{
    system("@cls||clear");
    initLogFile("server.log");
    //Creates a thread responsible for the menu console.
    pthread_t menu_thread_id;
    pthread_create(&menu_thread_id, NULL, menuThreadFunc, NULL);


    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    //Create the socket.
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    // Configure settings of the server address struct
    // Address family = Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(7799);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
    //Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind the address struct to the socket
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //Listen on the socket, with 40 max connection requests queued

    if(listen(serverSocket, 50)==0)
        info("Server started. Listening...");
    else
        error("Server Error");

  //  char buffer[1024];
    //char message[1000];
    pthread_t tid[60];
    int i = 0;
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        info("New client connected, creating a thread");
        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if( pthread_create(&tid[i], NULL, playerThreadFunc, &newSocket) != 0 )
            error("Failed to create thread\n");
        if( i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
    }

    info("Server shutting down...");
    close(serverSocket);

    return 0;
}
