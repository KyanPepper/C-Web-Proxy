#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#define MAX_REQUEST 5012
#define PATH_LEN 256
#define HOST_LEN 128
#define METHOD_LEN 10

// Log error message in console
void error(const char *msg);

// On client error, close the client socket, keep the server socket open
void error_on_client(const char *msg);

// Create Socket and bind to a port and listen for incoming connections
void proxy(int port);

// Close the proxy server
void close_proxy(int sig);

// parse the request to get the host and path
int parse_http_request(char *request, char *method, char *host, char *path);

// Send the request to the server and get the response
int send_request(int client_sock, char *host, char *path);

// Control flow to receive and send data to client
void handle_client(int client_sock);

// Write to client socket
void write_to_client_socket(int client_sock, char *buff);
