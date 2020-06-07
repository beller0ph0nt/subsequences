#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include <common-library.h>
#include <protocol.h>
#include <trace.h>



void close_connection(int socket) {
    TRACE_DEBUG("close connection");
    shutdown(socket, SHUT_RDWR);
    close(socket);
}
