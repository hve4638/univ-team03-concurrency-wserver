#ifndef __REQPHASE__
#define __REQPHASE__
#include "RequestInfo.h"

void req_phase_read(RequestInfo*);
void req_phase_process(RequestInfo*);
void req_phase_finished(RequestInfo*);
#endif