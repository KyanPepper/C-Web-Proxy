#include "proxy.h"
// Example http request (this is not a comment but for me to remember the format)
// HTTP/1.0 200 OK\r\n
// Content-Type: text/html\r\n
// \r\n
// <html><body>Hello, world!</body></html>

volatile sig_atomic_t stop = 1;

// Log error message in console
void error(const char *msg)
{

    FILE *log_file = fopen(ERROR_LOG, "a");
    if (log_file == NULL)
    {
        perror("Error opening log file");
        return;
    }
    fprintf(log_file, "%s\n", msg);
    fclose(log_file);
}

void log_action(const char *msg, int socketId)
{
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL)
    {
        perror("Error opening log file");
        return;
    }
    fprintf(log_file, "Socket %d %s\n", socketId, msg);
    fclose(log_file);
}

void error_on_client(const char *msg, int client_sock)
{
    int newMessageSize = strlen(msg) + 50;
    char newMessage[newMessageSize];
    strcpy(newMessage, msg);
    snprintf(newMessage + strlen(newMessage), newMessageSize - strlen(newMessage),
             " error on client socket %d", client_sock);

    error(newMessage);
    write(client_sock, "HTTP/1.0 400 Bad Request\r\n", 25);
    close(client_sock);
}

void close_proxy(int sig)
{
    (void)sig; // Cast to void to avoid unused parameter warning
    stop = 0;
    printf("Stopping Server: \n");
    exit(0);
}

int parse_http_request(char *domain, size_t domain_size,
                       char *port, size_t port_size,
                       char *path, size_t path_size,
                       const char *buffer)
{
    // Ensure the request starts with "GET http://"
    if (strncmp(buffer, "GET http://", 11) != 0)
    {
        return 0;
    }

    // Move the pointer to the start of the URL (just past "GET http://")
    const char *url_start = buffer + 11;

    // Locate the end of the URL (marked by the first space)
    const char *url_end = strchr(url_start, ' ');
    if (!url_end)
    {
        return 0; // Invalid format, no space after the URL
    }

    // Extract the domain, port, and path
    const char *path_start = strchr(url_start, '/'); // Path starts with '/'
    const char *port_start = strchr(url_start, ':');

    // Extract domain and port
    if (port_start && (!path_start || port_start < path_start))
    {
        // If a port is specified, copy the domain up to the ':'
        size_t domain_length = port_start - url_start;
        if (domain_length >= domain_size)
        {
            return 0; // Domain buffer too small
        }
        strncpy(domain, url_start, domain_length);
        domain[domain_length] = '\0';

        // Copy the port up to the '/' or end of the URL
        port_start++; // Move past ':'
        size_t port_length = path_start ? (size_t)(path_start - port_start) : (size_t)(url_end - port_start);
        if (port_length >= port_size)
        {
            return 0; // Port buffer too small
        }
        strncpy(port, port_start, port_length);
        port[port_length] = '\0';
    }
    else
    {
        // No port specified, default to "80 for http
        size_t domain_length = path_start ? (size_t)(path_start - url_start) : (size_t)(url_end - url_start);
        if (domain_length >= domain_size)
        {
            return 0; // Domain buffer too small
        }
        strncpy(domain, url_start, domain_length);
        domain[domain_length] = '\0';

        strncpy(port, "80", port_size - 1);
        port[port_size - 1] = '\0';
    }

    // Extract the path
    if (path_start && path_start < url_end)
    {
        size_t path_length = (size_t)(url_end - path_start);
        if (path_length >= path_size)
        {
            return 0; // Path buffer too small
        }
        strncpy(path, path_start, path_length);
        path[path_length] = '\0';
    }
    else
    {
        // Default to "/"
        strncpy(path, "/", path_size - 1);
        path[path_size - 1] = '\0';
    }

    return 1;
}

void handle_client(int client_sock)
{
    // Buffer to store the request
    char buffer[MAX_REQUEST];

    // Read the request from the client
    int n = read(client_sock, buffer, MAX_REQUEST);
    if (n < 0)
    {
        error_on_client("Error:HANDLE_CLIENT reading from socket", client_sock);
        return;
    }

    log_action(buffer, client_sock);

    // Parse the request to get the host and path
    char domain[256];
    char port[10];
    char path[1024];

    if (parse_http_request(domain, sizeof(domain), port, sizeof(port), path, sizeof(path), buffer) != 1)
    {
        error_on_client("Error:HANDLE_CLIENT parsing request", client_sock);
        return;
    }

    log_action("Request Parsed", client_sock);

    // Send the request to the server and get the response assuming get request
    if (send_request(client_sock, domain, path, port) < 0)
    {
        error_on_client("Error:HANDLE_CLIENT sending request", client_sock);
        return;
    }
    log_action("Content Received", client_sock);

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

int send_request(int client_sock, char *domain, char *path, char *port)
{

    // Create a socket for the server (TCP)
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        error("Error:SEND_REQUEST opening socket");
        return -1;
    }

    // Get the server's IP address
    struct hostent *server = gethostbyname(domain);
    if (server == NULL)
    {
        error("Error:SEND_REQUEST no such host");
        return -1;
    }

    // Initialize the server address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port));
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    // Connect to the server
    if (connect(server_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Error:SEND_REQUEST connecting");
        return -1;
    }

    // Send the request to the server
    char request[1024];
    snprintf(request, sizeof(request), "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, domain);
    int n = write(server_sock, request, strlen(request));
    if (n < 0)
    {
        error("Error:SEND_REQUEST writing to socket");
        return -1;
    }

    // Read the response from the server and write it to the client
    char buffer[1024];
    while ((n = read(server_sock, buffer, sizeof(buffer))) > 0)
    {
        write_to_client_socket(client_sock, buffer);
    }

    if (n < 0)
    {
        error("Error:SEND_REQUEST reading from socket");
        return -1;
    }

    close(server_sock);
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

        log_action("Connection accepted", clientSocket);

        handle_client(clientSocket);
    }

    close(proxySocket);
    exit(0);
}
