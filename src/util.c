#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


int 
so_nonblock(int fd)
{
    int flags, rc;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        fprintf(stdout, "fcntl(F_GETFL) failed: %s\n", strerror(errno));
        return -1;
    }

    flags |= O_NONBLOCK;
    rc = fcntl(fd, F_SETFL, flags);
    if (rc < 0) {
        fprintf(stdout, "fcntl(F_SETFL) failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}


int 
so_reuseaddr(int fd)
{
    int rc, optv;

    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optv, sizeof(optv));

    if (rc < 0) {
        fprintf(stdout, "SO_REUSEADDR failed: %s\n", strerror(errno));
    }

    return rc;
}

