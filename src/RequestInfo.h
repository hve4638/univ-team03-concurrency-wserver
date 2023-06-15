#ifndef __REQUESTINFO__
#define __REQUESTINFO__
enum RequestPhase {
    REQ_PHASE_READ,
    REQ_PHASE_PROCESS,
    REQ_PHASE_FINISHED,
};

typedef struct RequestInfo {
    int phase;
    int fd;
    char *method;
    char *uri;
    char *version;
} RequestInfo;
#endif