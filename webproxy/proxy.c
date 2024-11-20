#include "proxy.h"
// Example http request (this is not a comment but for me to remember the format)
// HTTP/1.0 200 OK\r\n
// Content-Type: text/html\r\n
// \r\n
// <html><body>Hello, world!</body></html>

volatile sig_atomic_t stop = 1;

void error(const char *msg)
{
    perror(msg);
}
void error_on_client(const char *msg)
{
    perror(msg);
    exit(1);
}

void close_proxy(int sig)
{
    (void)sig; // Cast to void to avoid unused parameter warning
    stop = 0;
    printf("Stopping Server: \n");
}

int parse_http_request(char *request, char *method, char *host, char *path)
{
    printf("Request: %s\n", request);
    // Parse the request to get the host and path
    int good = sscanf(request, "%9s http://%127s[^/]%255s", method, host, path);
    // only GET method is supported
    if (strlen(path) == 0)
    {
        strcpy(path, "/");
    }

    // bad format 401
    if (good != 3)
    {
        error("Error:PARSE_HTTP_REQUEST parsing request");
        return -1;
    }

    if (strcmp(method, "GET") != 0)
    {
        error("ERROR:PARSE_HTTP_REQUEST Only GET method is supported");
        return -1;
    }
    request[strlen(request) - 1] = '\0'; // null terminate the string
    host[strlen(host) - 1] = '\0';
    path[strlen(path) - 1] = '\0';

    return 0;
}

void handle_client(int client_sock)
{
    // Buffer to store the request
    char buffer[MAX_REQUEST];

    // Read the request from the client
    int n = read(client_sock, buffer, MAX_REQUEST);
    if (n < 0)
    {
        error("Error:HANDLE_CLIENT reading from socket");
    }

    // Parse the request to get the host and path
    char method[METHOD_LEN], host[HOST_LEN], path[PATH_LEN];

    if (parse_http_request(buffer, method, host, path) < 0)
    {
        error("Error:HANDLE_CLIENT parsing request");
    }

    // Send the request to the server and get the response assuming get request
    send_request(client_sock, host, path);

    // Close the client socket  (This indicates end of http request (TCP))
    close(client_sock);
}

void write_to_client_socket(int client_sock, char *content)
{

    int n = write(client_sock, content, strlen(content));
    if (n < 0)
    {
        error("Error:WRITE_TO_CLIENT_SOCKETWRITE_TO_CLIENT_SOCKET writing to socket");
    }
}

int send_request(int client_sock, char *host, char *path)
{
    // Create a tcp socket
    int serversocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket < 0)
    {
        error("Error: SEND REQUEST opening socket");
        return -1;
    }

    // Get the server's IP address
    struct hostent *server = gethostbyname(host);
    if (server == NULL)
    {
        error("Error: SEND REQUEST, no such host");
    }

    // Initialize the server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    // Connect to the server
    if (connect(serversocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Error: Send Request connecting to server");
    }

    // Send the request to the server
    char request[MAX_REQUEST];
    snprintf(request, MAX_REQUEST, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
    int n = write(serversocket, request, strlen(request));
    if (n < 0)
    {
        error("Error:SEND_REQUEST writing to socket");
    }

    // Buffer to store the response
    char buff[MAX_REQUEST]; // +1 for null terminator
    int total_read = 0;

    // Read the response from the server in chunks
    while ((n = read(serversocket, buff, MAX_REQUEST)) > 0)
    {

        write_to_client_socket(client_sock, buff + total_read);
    }
    if (n < 0)
    {
        // potential return 404?
        error("Error:SEND_REQUEST nothing read from socket");
    }
    // Close the server socket
    close(serversocket);

    return 0;
}

void proxy(int port)
{
    // Register signal handler for closing the proxy server
    signal(SIGINT, close_proxy);

    // Creates a tcp socket (for http)
    int proxySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxySocket < 0)
    {
        // If socket creation fails, display an error message and exit
        error("Error:PROXY opening socket");
    }

    // Initialize server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;         // Use the IPv4 address family
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Bind to anything on local system
    serv_addr.sin_port = htons(port);       // Convert the port to big endian (network byte order)

    // Bind socket to address
    if (bind(proxySocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Error:PROXY on binding");
    }
    printf("Serving on port %d\n", port);

    // Listen for incoming connections
    // This marks the socket as a passive socket that will be used to accept incoming connections.
    listen(proxySocket, 5);

    while (stop == 1)
    {
        // Accept incoming connections
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int clientSocket = accept(proxySocket, (struct sockaddr *)&cli_addr, &clilen);

        if (clientSocket < 0)
        {
            error("Error:PROXY on accepting connection");
        }

        printf("Connection accepted\n");

        handle_client(clientSocket);
    }

    close(proxySocket);
    exit(0);
}
