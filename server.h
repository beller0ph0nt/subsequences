#ifndef __SERVER_H
#define __SERVER_H



enum {
    SEQUENCE_COUNT = 3
};



typedef struct thread_arguments {
    int socket;
} thread_arguments_t;

#endif // __SERVER_H
