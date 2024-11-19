#include "proxy.h"


int main(int argc, char *argv[])
{
    // Check usage
    if (argc < 2)
    {
        printf("Provide a port number\n");
        exit(1);
    }

    // Get port
    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535)
    {
        printf("Port number must be between 1024 and 65535\n");
        exit(1);
    }

    // Start the proxy server
    proxy(port);

    return 0;
}