#include "proxy.h"
volatile sig_atomic_t stop = 1;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void close_proxy(int sig)
{
    stop = 0;
    printf("Stopping Server: \n");
}

void proxy(int port)
{

    // Register signal handler for closing the proxy server
    signal(SIGINT, close_proxy);

    // Creates a tcp socket (for http)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        // If socket creation fails, display an error message and exit
        error("Error opening socket");
    }

    // Initialize server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;         // Use the IPv4 address family
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Bind to anything on local system
    serv_addr.sin_port = htons(port);       // Convert the port to big endian (network byte order)

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Error on binding");
    }

    // Listen for incoming connections
    // This marks the socket as a passive socket that will be used to accept incoming connections.
    listen(sockfd, 5);

    // Accept incoming connections
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); //control flow blocked here (no async await)

    if (newsockfd < 0)
    {
        error("Error on accepting connection");
    }

    while (stop == 1)
    {

    }
    // Close the sockets
    close(newsockfd);
    close(sockfd);
    exit(0);
}
