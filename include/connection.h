#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "server.h"


int connection_process(server_t *server, int fd, int events);


#endif /*__CONNECTION_H__*/

