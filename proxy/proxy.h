#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>

// Log error message in console
void error(const char *msg);

// Create Socket and bind to a port and listen for incoming connections
void proxy(int port);

// Close the proxy server
void close_proxy(int sig);

//parse the request to get the host and path
void parse_request(char *buffer, char *host, char *path);

void parse_response(char *buffer, char *status, char *content);

// Send the request to the server and get the response
void send_request(int client_sock, char *host, char *path);

// Send the response to the client
void send_response(int client_sock, char *status, char *content);




