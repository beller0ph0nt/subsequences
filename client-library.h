#ifndef __CLIENT_LIBRARY_H
#define __CLIENT_LIBRARY_H

#include <protocol.h>



void usage(const char* application_name);
void read_answer(const int socket, answer_t* answer);
void fill_reqest(reqest_t* reqest, const uint8_t reqest_type, const uint64_t start_value, const uint64_t step);
void write_reqest(int socket, reqest_t* reqest);
int  open_client_connection(const char* ip, int port);

#endif // __CLIENT_LIBRARY_H
