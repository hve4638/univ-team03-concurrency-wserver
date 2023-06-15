#include "reqphase.h"
#include "request.h"
#include "io_helper.h"
#include "tqueue.h"

void req_phase_read(RequestInfo* info) {
    char buf[MAXBUF], method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    int fd = info->fd;
    
    readline_or_die(fd, buf, MAXBUF);
    sscanf(buf, "%s %s %s", method, uri, version);
    
    info->phase = REQ_PHASE_PROCESS;
    info->method = strdup(method);
    info->uri = strdup(uri);
    info->version = strdup(version);
}

void req_phase_process(RequestInfo* info) {
    int is_static;
    struct stat sbuf;
    char filename[MAXBUF], cgiargs[MAXBUF];

    int fd = info->fd;
    char *method = info->method;
    char *uri = info->uri;
    char *version = info->version;
    info->phase = REQ_PHASE_FINISHED;
    //printf("process phase(%d)\n", info->fd);
    //printf("method:%s uri:%s version: %s\n", method, uri, version);
    
    if (strcasecmp(method, "GET")) {
	request_error(fd, method, "501", "Not Implemented", "server does not implement this method");
	return;
    }
    request_read_headers(fd);
    
    is_static = request_parse_uri(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
	request_error(fd, filename, "404", "Not found", "server could not find this file");
	return;
    }
    
    if (is_static) {
	if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
	    request_error(fd, filename, "403", "Forbidden", "server could not read this file");
	    return;
	}
	request_serve_static(fd, filename, sbuf.st_size);
    } else {
	if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
	    request_error(fd, filename, "403", "Forbidden", "server could not run this CGI program");
	    return;
	}
	request_serve_dynamic(fd, filename, cgiargs);
    }
}

void req_phase_finished(RequestInfo* info) {
    //printf("process finished(%d)\n", info->fd);
}