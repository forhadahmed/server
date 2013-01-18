#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/epoll.h>

/*
 * The server_t type is simply a socket (fd) listening on a (port) and an 
 * associated epoll descriptor / queue (eq) that notifies the code when events
 * arrive on the listening socket (and possibly any client sockets)
 */
#define epev struct epoll_event
typedef struct server_ {
    int   fd;
    int   eq;
    epev *ev;
    short port;
    int   flags;
    int   index;
} server_t;


int server_init(server_t *s, int port);

int server_run(server_t *s);

int server_exit(server_t *s);


#endif/*__SERVER_H__*/
