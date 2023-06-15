#include <stdio.h>
#include "reqthread.h"
#include "request.h"
#include "io_helper.h"

char default_root[] = ".";

enum SCHEDULING {
	SCHEDULING_SFF,
	SCHEDULING_FIFO,
};

//
// ./wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	int thread_count = 1;
	int buffer_size = 1;
	int sche = SCHEDULING_FIFO;

    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1) {
		switch (c) {
			case 'd':
				root_dir = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 't':
				thread_count = atoi(optarg);
				break;
			case 'b':
				buffer_size = atoi(optarg);
				break;
			case 's':
				if (strcmp(optarg, "FIFO") == 0) {
					sche = SCHEDULING_FIFO;
				}
				else if (strcmp(optarg, "SFF") == 0) {
					sche = SCHEDULING_SFF;
				}
				break;
			default:
				fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]\n");
				exit(1);
		}
	}

	max_requests_buffer = buffer_size;
	reqthread_init();

	pthread_t *preworker;
	pthread_t *worker;
	preworker = preworker_create(thread_count);
	worker = worker_create(thread_count);

    chdir_or_die(root_dir);

	int fifo_count = 0;
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		
		RequestInfo *info = (RequestInfo*)malloc(sizeof(RequestInfo));
		info->phase = REQ_PHASE_READ;
		info->fd = conn_fd;
		info->method = NULL;
		info->uri = NULL;
		info->version = NULL;
		
		int num = requests_add(info);
		if (sche == SCHEDULING_SFF) {
			tqueue_push(&prework_queue, num);
		}
		else { //FIFO
			tpqueue_push(&work_queue, num, fifo_count++);
		}
		
    }
    return 0;
}