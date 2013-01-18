#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server.h"

static char buf[128000];
static uint64_t total = 0;

int
connection_process(server_t *server, int fd, int events)
{
    int rc = 0;
    struct epoll_event ev;
    struct sockaddr caddr;
    socklen_t slen;  
     
    while (1) {

        rc = read(fd, buf, sizeof(buf));

        if (rc > 0) {

            total += rc;

        } else if (rc == 0) {

            fprintf(stdout, "read(%d) remote close\n", fd);
            close(fd);
            break;

        } else {

            if (errno != EAGAIN) {
                fprintf(stdout, "read(%d) failed\n", fd);
                close(fd);
            }

            break;
        }
    }
            
    if ((total/1000000)%2 == 0) fprintf(stdout, "r: %llu\n", total);
}

