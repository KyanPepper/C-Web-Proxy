#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define ERROR_LOG "proxy_error.log"
#define LOG_FILE "proxy.log"
#define MAX_REQUEST 5012
#define PATH_LEN 256
#define HOST_LEN 128
#define METHOD_LEN 10

// Log requests in file
void log_action(const char *msg, int id);

// Log error message in console
void error(const char *msg);

// On client error, close the client socket, keep the server socket open
void error_on_client(const char *msg, int client_sock);

// Create Socket and bind to a port and listen for incoming connections
void proxy(int port);

// Close the proxy server
void close_proxy(int sig);

// parse the request to get the host and path
int parse_http_request(char *domain, size_t domain_size,
                       char *port, size_t port_size,
                       char *path, size_t path_size,
                       const char *buffer);

// Send the request to the server and get the response
int send_request(int client_sock, char *domain, char *path, char *port);

// Control flow to receive and send data to client
void handle_client(int client_sock);

// Write to client socket
void write_to_client_socket(int client_sock, char *buff);
