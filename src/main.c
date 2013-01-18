#include "server.h"

int main(int argc, char *argv[])
{
    server_t server;

    if (server_init(&server, 80) < 0) {
        return -1;
    }

    return server_run(&server);
}

