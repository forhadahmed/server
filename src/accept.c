#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "server.h"


/* 
 * o Single process Single thread
 *   queue the accepted fd to the same epoll queue as the server socket
 * o Single process Multiple threads 
 *   queue the accepted fd to a designated worker thread's epoll queue
 * o Multi process
 *   TBD...
 */
static int
queue_connection(server_t *server, int fd)
{
    int rc;
    struct epoll_event ev;

    rc = so_nonblock(fd);
 
    if (rc < 0) {
        return rc;
    }
 
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    rc = epoll_ctl(server->eq, EPOLL_CTL_ADD, fd, &ev);  
 
    if (rc < 0) {
        fprintf(stdout, "epoll_ctl(+fd) failed: %s\n", strerror(errno));
    }
}


/*
 * accept() connections from a server socket fd (sfd) and queue them somewhere
 */  
int 
accept_connections(server_t *server, int events)
{
    int fd, rc;
    struct sockaddr caddr;
    socklen_t slen;

    while (1) {

        slen = sizeof(caddr);

        fd = accept(server->fd, &caddr, &slen); 

        if (fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            fprintf(stdout, "accept() failed: %s\n", strerror(errno));
            break;
        }

        rc = queue_connection(server, fd);
    }
}


