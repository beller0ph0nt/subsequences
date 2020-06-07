#include <endian.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <common-library.h>
#include <server-library.h>
#include <trace.h>



void usage(char* application_name) {
    fprintf(stderr,"usage: %s <port_number>\n", application_name);
    exit(EXIT_FAILURE);
}



void read_reqest(int socket, reqest_t* reqest) {
    TRACE_DEBUG("read request");

    read(socket, &reqest->header.reqest_type, sizeof(uint8_t));
    if (reqest->header.reqest_type == REQ_EXPORT_TYPE) {
        reqest->body.start_value = reqest->body.step = 0;
    } else {
        read(socket, &reqest->body.start_value, sizeof(uint64_t));
        reqest->body.start_value = be64toh(reqest->body.start_value);
        read(socket, &reqest->body.step, sizeof(uint64_t));
        reqest->body.step = be64toh(reqest->body.step);
    }

    TRACE_DEBUG("  header");
    TRACE_DEBUG("    reqest_type: %d", reqest->header.reqest_type);
    TRACE_DEBUG("  body");
    TRACE_DEBUG("    start_value: %lu", reqest->body.start_value);
    TRACE_DEBUG("    step: %lu", reqest->body.step);
}



void write_answer(const int socket, const answer_t* answer) {
    TRACE_DEBUG("write answer number: %lu", answer->number);
    write(socket, &answer->is_end, sizeof(uint8_t));
    uint64_t number = htobe64(answer->number);
    write(socket, &number, sizeof(uint64_t));
}



void write_end_answer(const int socket) {
    answer_t answer = { .is_end = TRUE, .number = 0 };
    write_answer(socket, &answer);
}



int create_server_connection(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        TRACE_ERROR("socket() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset((char*) &server_address, 0, sizeof(server_address));
    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port        = htons(port);

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        TRACE_ERROR("bind() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    TRACE_DEBUG("listening...");
    listen(server_socket, 1);

    return server_socket;
}



int accept_reqest(int sock) {
    TRACE_DEBUG("accepting request...");
    struct sockaddr_in cli_addr;
    socklen_t sock_len = sizeof(cli_addr);
    int newsock = accept(sock, (struct sockaddr *) &cli_addr, &sock_len);
    if (newsock < 0) {
        TRACE_ERROR("accept() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return newsock;
}
