#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>





// Function prototypes
void error(const char *msg);
void proxy(int port);
void connectToServer(int clientSocket, char *host, int port);
void connectToClient(int serverSocket, int clientSocket);


