#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "reqthread.h"
#define MAXBUF (8192)

extern void request_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
extern void request_read_headers(int fd);
extern int request_parse_uri(char *uri, char *filename, char *cgiargs);
extern void request_get_filetype(char *filename, char *filetype);
extern void request_serve_dynamic(int fd, char *filename, char *cgiargs);
extern void request_serve_static(int fd, char *filename, int filesize);

extern int bufferCounter;

#endif // __REQUEST_H__
