#include <endian.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <common-library.h>
#include <client-library.h>
#include <protocol.h>
#include <trace.h>



void usage(const char* application_name) {
    fprintf(stderr, 
            "usage: %s <ip> <port> { export seq | req[1-3] <start> <step> }\n",
            application_name);
    exit(EXIT_FAILURE);
}



void read_answer(const int socket, answer_t* answer) {
    TRACE_DEBUG("read answer %p", answer);
    read(socket, &answer->is_end, sizeof(uint8_t));
    read(socket, &answer->number, sizeof(uint64_t));
    answer->number = be64toh(answer->number);    

    TRACE_DEBUG("  answer");
    TRACE_DEBUG("    is_end: %d", answer->is_end);
    TRACE_DEBUG("    number: %lu", answer->number);
}



void fill_reqest(reqest_t* reqest,
                 const uint8_t reqest_type,
                 const uint64_t start_value,
                 const uint64_t step) {
    TRACE_DEBUG("fill request %p", reqest);

    reqest->header.reqest_type = reqest_type;
    reqest->body.start_value = htobe64(start_value);
    reqest->body.step = htobe64(step);

    TRACE_DEBUG("request");
    TRACE_DEBUG("  header");
    TRACE_DEBUG("    reqest_type: %d", reqest_type);
    TRACE_DEBUG("  body");
    TRACE_DEBUG("    start_value: %lu", start_value);
    TRACE_DEBUG("    step: %lu", step);
}



void write_reqest(int socket, reqest_t* reqest) {
    TRACE_DEBUG("write request %p", reqest);
    write(socket, &reqest->header.reqest_type, sizeof(header_t));
    write(socket, &reqest->body.start_value, sizeof(uint64_t));
    write(socket, &reqest->body.step, sizeof(uint64_t));
}



int open_client_connection(const char* ip, const int port) {
    TRACE_DEBUG("open client connection");
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        TRACE_ERROR("socket() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    struct in_addr in;
    if (inet_aton(ip, &in) == 0) {
        TRACE_ERROR("invalid address");
        exit(EXIT_FAILURE);
    }
    server_address.sin_addr.s_addr = in.s_addr;
    server_address.sin_port = htons(port);

    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        TRACE_ERROR("connect() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return client_socket;
}
