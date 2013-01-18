#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"
#include "accept.h"
#include "server.h"
#include "connection.h"


#define LISTEN_BACKLOG 4096
#define EPOLL_MAX      16384
#define SERVER_MAX     64


static server_t *servers[SERVER_MAX];
static int       nservers = 0;


int 
server_init(server_t *server, int port)
{
    int rc = 0;
    struct epoll_event ev;
    struct sockaddr_in saddr;

    server->port = port;

    memset(&saddr, 0, sizeof(saddr)); 
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(server->port);

    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server->fd < 0) {
        fprintf(stdout, "socket() failed: %s\n", strerror(errno));
        exit(1);
    }

    rc = so_reuseaddr(server->fd);

    if (rc < 0) {
        return rc;
    }

    rc = bind(server->fd, (struct sockaddr *) &saddr, sizeof(saddr));

    if (rc < 0) {
        fprintf(stdout, "bind() failed: %s\n", strerror(errno));
        return rc;
    }
 
    rc = listen(server->fd, LISTEN_BACKLOG);

    if (rc < 0) {
        fprintf(stdout, "listen() failed: %s\n", strerror(errno));
        return rc;
    }
    
    rc = so_nonblock(server->fd);

    if (rc < 0) {
        return rc;
    }
 
    server->eq = epoll_create1(0);

    if (server->eq < 0) {
        fprintf(stdout, "epoll_create1() failed: %s\n", strerror(errno));
        return -1;
    }

    ev.data.fd = server->fd;
    ev.events = EPOLLIN | EPOLLET;
   
    rc = epoll_ctl(server->eq, EPOLL_CTL_ADD, server->fd, &ev);

    if (rc < 0) {
        fprintf(stdout, "epoll_ctl(+sfd) failed: %s\n", strerror(errno));
        return rc;
    }

    server->ev = calloc(EPOLL_MAX, sizeof(ev));

    if (server->ev == NULL) {
        fprintf(stdout, "calloc(EPOLL_MAX) failed\n");
        return -1;
    }

    servers[nservers++] = server;

    return rc;
}


int 
server_run(server_t *server)
{
    int i, e, n, fd;

    fprintf(stdout, "Listening on [%d]\n", server->port);

    while (1) {
        /*
         * Main blocking call
         */
        n = epoll_wait(server->eq, server->ev, EPOLL_MAX, -1);

        for (i = 0; i < n; i++) {
  
            e = server->ev[i].events; 
            fd = server->ev[i].data.fd;

            if ((e & EPOLLERR) || (e & EPOLLHUP)) {
                /*
                 * Error
                 */
                continue;
            } 
 
            if (fd == server->fd) {

                accept_connections(server, e);

            } else {
                /*
                 * We are processing client connections in the same thread as 
                 * the main server thread
                 */
                connection_process(server, fd, e);
            } 
        }
    }

    close(server->fd);
    close(server->eq);
    free(server->ev);
}    

            

            


