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

    // Run proxy
    proxy(port);

    return 0;
}