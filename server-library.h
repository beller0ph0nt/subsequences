#ifndef __SERVER_LIBRARY_H
#define __SERVER_LIBRARY_H

#include <protocol.h>



typedef struct {
    uint64_t start_value;
    uint64_t step;
} sequence_t;



void usage(char* application_name);
void read_reqest(int socket, reqest_t* reqest);
void write_answer(const int sock, const answer_t* answer);
void write_end_answer(const int socket);
int create_server_connection(int port);
int accept_reqest(int socket);

#endif // __SERVER_LIBRARY_H
