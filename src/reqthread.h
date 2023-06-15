#ifndef __REQTHREAD__
#define __REQTHREAD__
#include "tqueue.h"
#include "tpqueue.h"
#include "RequestInfo.h"
#define MAX_REQUEST_BUFFER (512)

extern size_t max_requests_buffer;
extern RequestInfo* requests[MAX_REQUEST_BUFFER];
extern TQueue queue;
extern TQueue prework_queue;
extern TPQueue work_queue;

pthread_t* preworker_create(int);
pthread_t* worker_create(int);

void reqthread_init();
void reqcomsume_create(pthread_t*);

void req_process_read(RequestInfo*);
void req_process_process(RequestInfo*);
#endif