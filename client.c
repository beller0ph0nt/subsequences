#include <errno.h>
#include <protocol.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>

#include <common-library.h>
#include <client-library.h>
#include <errors.h>



int main(int argc, char** argv) {
    if (argc < 5 || 6 < argc) {
        usage(argv[0]);
        exit(ERROR_UNKNOWN_REQEST);
    }

    const char* application_name = argv[0];
    const char* host = argv[1];
    const int port = atoi(argv[2]);

    if (argc == 5) {
        if (strcmp(argv[3], "export") == 0 &&
            strcmp(argv[4], "seq") == 0) {
            int socket = open_client_connection(host, port);

            reqest_t reqest;
            fill_reqest(&reqest, REQ_EXPORT_TYPE, 0, 0);
            write_reqest(socket, &reqest);

            answer_t answer;
            while (1) {
                read_answer(socket, &answer);
                if (answer.is_end == TRUE) {
                    break;
                }
            }

            close_connection(socket);
        } else {
            usage(application_name);
        }
    } else if (argc == 6) {
        uint64_t start_value = strtoull(argv[4], NULL, 10);
        if (errno == ERANGE) {
            perror(strerror(errno));
            exit(ERROR_START_VALUE);
        }

        uint64_t step = strtoull(argv[5], NULL, 10);
        if (errno == ERANGE) {
            perror(strerror(errno));
            exit(ERROR_STEP_VALUE);
        }

        uint8_t reqest_type;
        if (strcmp(argv[3], "seq1") == 0) {
            reqest_type = REQ_1_TYPE;
        } else if (strcmp(argv[3], "seq2") == 0) {
            reqest_type = REQ_2_TYPE;
        } else if (strcmp(argv[3], "seq3") == 0) {
            reqest_type = REQ_3_TYPE;
        } else {
            usage(application_name);
            exit(ERROR_UNKNOWN_REQEST);
        }

        int socket = open_client_connection(host, port);

        reqest_t reqest;
        fill_reqest(&reqest, reqest_type, start_value, step);
        write_reqest(socket, &reqest);

        close_connection(socket);
    }

    return EXIT_SUCCESS;
}
