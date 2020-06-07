#ifdef __ATOMIC_ENABLED__
#include <atomic.h>
#elif __cplusplus
#include <atomic>
#endif

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <trace.h>
#include <sys/socket.h>

#include <server.h>
#include <server-library.h>
#include <common-library.h>



static volatile char is_working = TRUE;

static pthread_mutex_t g_sequence_lock;
static sequence_t g_sequence[SEQUENCE_COUNT];

#ifdef __ATOMIC_ENABLED__
static uint64_t g_threads_counter = 0;
#elif __cplusplus
static std::atomic<uint64_t> g_threads_counter = 0;
#else
static uint64_t g_threads_counter = 0;
#endif

static pthread_mutex_t g_threads_counter_lock;
static pthread_cond_t g_cv_threads_counter;




static void* thread_start(void* arguments) {
    thread_arguments_t* thread_arguments = arguments;
    int socket = thread_arguments->socket;
    free(thread_arguments);

#ifdef __ATOMIC_ENABLED__
    __atomic_add_fetch(&g_threads_counter, 1, __ATOMIC_ACQ_REL);
#elif __cplusplus
    g_threads_counter++;
#else
    pthread_mutex_lock(&g_threads_counter_lock);
    g_threads_counter++;
    pthread_mutex_unlock(&g_threads_counter_lock);
#endif

    sequence_t sequence[SEQUENCE_COUNT];

    pthread_mutex_lock(&g_sequence_lock);
    memcpy(&sequence, &g_sequence, sizeof(g_sequence));
    pthread_mutex_unlock(&g_sequence_lock);
    
    reqest_t request;
    read_reqest(socket, &request);
    if (request.header.reqest_type == REQ_EXPORT_TYPE) {
        uint8_t i;
        uint64_t sequence_value[SEQUENCE_COUNT];
        char is_all_sequence_empty = TRUE;
                
        for (i = 0; i < SEQUENCE_COUNT; i++) {
            if (sequence[i].start_value != 0 && sequence[i].step != 0) {
                is_all_sequence_empty = FALSE;
                sequence_value[i] = sequence[i].start_value;
            } else {
                sequence_value[i] = 0;
            }
        }
    
        i = 0;
        answer_t answer;
        while (is_working && !is_all_sequence_empty) {
            if (sequence[i].start_value != 0 && sequence[i].step != 0) {
                answer.is_end = FALSE;
                answer.number = sequence_value[i];

                write_answer(socket, &answer);
                
                if (sequence_value[i] < UINT64_MAX - sequence[i].step) {
                    sequence_value[i] += sequence[i].step;
                } else {
                    sequence_value[i] = sequence[i].start_value;
                }
            }

            i = (i < SEQUENCE_COUNT - 1) ? i + 1 : 0;
            
            usleep(300);
        }

        write_end_answer(socket);
    } else {
        int32_t i = -1;
        if (request.header.reqest_type == REQ_1_TYPE) {
            i = 0;
        } else if (request.header.reqest_type == REQ_2_TYPE) {
            i = 1;
        } else if (request.header.reqest_type == REQ_3_TYPE) {
            i = 2;
        }

        if (0 <= i) {
            pthread_mutex_lock(&g_sequence_lock);
            sequence[i].start_value = g_sequence[i].start_value = request.body.start_value;
            sequence[i].step = g_sequence[i].step = request.body.step;
            pthread_mutex_unlock(&g_sequence_lock);
        } else {
            TRACE_ERROR("unknown request type");
        }
    }

    close_connection(socket);
    
#ifdef __ATOMIC_ENABLED__
    __atomic_sub_fetch(&g_threads_counter, 1, __ATOMIC_ACQ_REL);
#elif _cplusplus
    g_threads_counter--;
#else
    pthread_mutex_lock(&g_threads_counter_lock);
    g_threads_counter--;
    pthread_mutex_unlock(&g_threads_counter_lock); 
#endif

    pthread_cond_signal(&g_cv_threads_counter);

    return NULL;
}



static void wait_threads() {
    pthread_mutex_lock(&g_threads_counter_lock);
#ifdef __ATOMIC_ENABLED__
    while (__atomic_load_n(&g_threads_counter, __ATOMIC_ACQ_REL) != 0)
#elif _cplusplus
    while (g_threads_counter.load() != 0)
#else
    while (g_threads_counter != 0)
#endif
    {
        pthread_cond_wait(&g_cv_threads_counter, &g_threads_counter_lock);
        TRACE_DEBUG("waiking...");
    }
    pthread_mutex_unlock(&g_threads_counter_lock);
}



static void SIGINT_handler(int signo) {
    TRACE_DEBUG("SIGINT Handler");
    is_working = FALSE;
    wait_threads();
}



int main(int argc, char** argv) {
    if (signal(SIGINT, SIGINT_handler) == SIG_ERR) {
        exit(EXIT_FAILURE);
    }

    if (argc < 2)
        usage(argv[0]);

    memset(&g_sequence, 0, sizeof(g_sequence));

    if (pthread_cond_init(&g_cv_threads_counter, NULL) != 0) {
        TRACE_ERROR("condition variable init");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int socket = create_server_connection(port);

    const int nfds = 1;
    struct pollfd fds = { .fd = socket, .events = POLLIN };
    const int mstimeout = 3 * 1000;

    while (is_working)
    {
        TRACE_DEBUG("polling...");
        int rc = poll(&fds, nfds, mstimeout);
        if (rc < 0) {
            TRACE_DEBUG("%s", strerror(errno));
            exit(EXIT_FAILURE);
        } else if (rc == 0) {
            TRACE_DEBUG("poll() timed out");
            continue;
        }

        int newsock = accept_reqest(socket);

        thread_arguments_t* arguments = calloc(1, sizeof(thread_arguments_t));
        arguments->socket = newsock;
        pthread_t thread;
        pthread_create(&thread, NULL, thread_start, (void*)arguments);
        pthread_detach(thread);
    }

    close_connection(socket);
    wait_threads();
    if (pthread_cond_destroy(&g_cv_threads_counter) != 0) {
        TRACE_ERROR("condition variable destroy");
        exit(EXIT_FAILURE);
    }

    return 0;
}
