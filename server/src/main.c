#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <pthread.h>
#include <getopt.h>

#include "draw.h"
#include "serial.h"
#include "logging.h"
#include "../include/game.h"
#include "../include/protocol.h"
#include "../include/menu.h"
#include "../include/player.h"

static struct option long_options[] =
{
    {"help",  no_argument, 0, 'h'},
    {"port",  required_argument, 0, 'p'},
    {"log",   required_argument, 0, 'l'},
    {0, 0, 0, 0}
};

void printUsage()
{
    printf("Unable to start the Minefield server.\n");
    printf("Usage: ./server --options--\n");
    printf("\tOptions are:\n"
           "\t\t--help: Print this message\n"
           "\t\t--port: (optional) listening port (default: 8000)\n"
           "\t\t--log:  (optional) the log filename (default ./server.log)\n");
}

/* S E R V E R */
int main(int argc, char *argv[ ])
{
    int option_index = 0;

    char *logFile = "server.log";
    int port = 8000;

    int c;
    while ((c = getopt_long(argc, argv, "h:p:l:", long_options, &option_index)) != -1) {
        switch (c)
        {
            case 'h':
              printUsage();
              exit(-1);
              break;

            case 'p':
              sscanf(optarg, "%d", &port);
              break;

            case 'l':
              logFile = optarg;
              break;
        }
    }

    system("@cls||clear");
    //Initializes the log file on which any activity will be logged
    initLogFile(logFile);

    infoMain("Starting Minefield server...");

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    //Create the server socket.
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int t = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

    //Creates a thread responsible for the menu console.
    pthread_t menu_thread_id;
    pthread_create(&menu_thread_id, NULL, menuThreadFunc, &serverSocket);

    // Configure settings of the server address struct
    // Address family = Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(port);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //any network interface address
    //Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind the address struct to the socket
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    //Listen on the socket, with 50 max connection requests queued
    if(listen(serverSocket, 50) == 0)
    {
        char logMessage[200];
        sprintf(logMessage, "Server started. Listening on port %d", port);
        infoMain(logMessage);
    }
    else
    {
        errorMain("Server Error");
    }

    //Loads the list of registered players (stored into a dedicated file)
    loadRegisteredPlayers("registered-players.db");

    while(1)
    {
        infoMain("Listening for client connections...");
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        infoMain("New client connected, creating a thread");

        pthread_t tid;
        //for each client request creates a thread
        if( pthread_create(&tid, NULL, playerThreadFunc, &newSocket) != 0 )
        {
            errorMain("Failed to create thread\n");
        }
        addPlayerThread(tid);
    }

    infoMain("Server shutting down...");
    shutdown(serverSocket, SHUT_RDWR);

    return 0;
}
