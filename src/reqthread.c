#include <sys/stat.h>
#include <limits.h>
#include "request.h"
#include "reqthread.h"
#include "reqphase.h"
#include "io_helper.h"

RequestInfo* requests[MAX_REQUEST_BUFFER];
TQueue queue;
TQueue prework_queue;
TPQueue work_queue;

static void req_prework_loop();
static void req_work_loop();
static void reqinfo_free(RequestInfo*);
static void tcreate(pthread_t*, void*);
static int getfilesize(const char*);
size_t max_requests_buffer = 1;

int requests_add(RequestInfo* info) {
    static int counter = 0;
    int spin_count = 0;
    // 꽉 차있다면 spin-loop
    while(1) {
        counter = (counter+1) % max_requests_buffer;

        if (requests[counter] == NULL) {
            break;
        }
        else if (++spin_count >= max_requests_buffer) {
            // 버퍼가 가득 찼을때 spin-loop로 인한 cpu낭비를 방지
            spin_count = 0;
            sched_yield();
        }
    }

    requests[counter] = info;
    return counter;
}

pthread_t* preworker_create(int count) {
    size_t size = sizeof(pthread_t) * (count+3);
    pthread_t* threads = (pthread_t*)malloc(size);
    memset(threads, 0, size);

    for (int i = 0; i < count; i++) {
        tcreate(&threads[i], req_prework_loop);
    }
    return threads;
}

pthread_t* worker_create(int count) {
    size_t size = sizeof(pthread_t) * (count+3);
    pthread_t* threads = malloc(size);
    memset(threads, 0, size);

    for (int i = 0; i < count; i++) {
        tcreate(&threads[i], req_work_loop);
    }
    return threads;
}

void reqthread_init() {
    memset(requests, NULL, sizeof(requests));
    tqueue_init(&queue);
    tqueue_init(&prework_queue);
    tpqueue_init(&work_queue, 512);
    bufferCounter = 0;
}

void tcreate(pthread_t *thread_t, void* call) {
    if (pthread_create(thread_t, NULL, call, NULL) < 0) {
        printf("Error : could not create thread\n");
        exit(0);
    }
}

// Producer worker 로직
void req_prework_loop() {
    int fd;
    char buf[MAXBUF], method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    char filename[MAXBUF], cgiargs[MAXBUF];
    RequestInfo* info;
    int filesize;

    while(1) {
        int index = tqueue_pop(&prework_queue);
        if (index == -1) {
            sched_yield();
            continue;
        }
        info = requests[index];

        if (info->phase == REQ_PHASE_READ) {
            req_phase_read(info);

            //printf("uri: %s\n", info->uri);
            filesize = getfilesize(info->uri);
        }
        else {
            // 정상적인 경우 실행되지 않음
            filesize = 0;
        }
		
        tpqueue_push(&work_queue, index, filesize);
    }
}

// Comsumer Worker 로직
// SFF, FIFO 에 따라 동작이 달라짐
void req_work_loop() {
    int index;
    RequestInfo* info;

    while(1) {
        index = tpqueue_pop(&work_queue);
        if (index == -1) {
            sched_yield();
            continue;
        }
        info = requests[index];
        requests[index] = NULL;

        if (info->phase == REQ_PHASE_READ) {
            req_phase_read(info);
        }
        if (info->phase == REQ_PHASE_PROCESS) {
            req_phase_process(info);
        }
        if (info->phase == REQ_PHASE_FINISHED) {
            req_phase_finished(info);
        }

        close_or_die(info->fd);
        reqinfo_free(info);
    }
}

void reqinfo_free(RequestInfo* info) {
    free(info->method);
    free(info->uri);
    free(info->version);
    free(info);
}

int getfilesize(const char* fname) {
    struct stat st;
    
    if (stat(fname+1, &st) != 0) {
        printf("couldnotfind\n");
        return 0;
    } else if (st.st_size < INT_MAX) {
        return (int)st.st_size;
    } else {
        // tpqueue에 들어가는 priority와 st_size의 자료형이 다르므로 int형으로 변환 필요
        // INT_MAX 바이트 이상 들어오는 경우가 많지 않으므로 INT_MAX이상 사이즈는 INT_MAX 리턴
        return INT_MAX;
    }
}