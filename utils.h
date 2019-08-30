

#ifndef UTILS_H
#define UTILS_H
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "stdbool.h"
#include <poll.h>
#include <string.h>
#include <signal.h>

#define MAX_LENGHT 100
#define BUFF_SIZE 150

#define DATA "data"
#define MASK 0777
#define UNIX_PATH_MAX 108
#define SOCKNAME "./objstore.sock"

#define CHECKNULL(s,e) \
    if (s==NULL) { perror(e);exit(errno); }
//funzione per essere sicuri di aver scritto tutti i byte
int check_answer(char* answer,char * e);
/*ssize_t writen(int fd ,void *buff,size_t len);
ssize_t readn(int fd ,void *buff,size_t len);*/
ssize_t read_to_new(int fd ,void *buff,size_t len);
static inline int readn(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
	if ((r=read((int)fd ,bufptr,left)) == -1) {
	    if (errno == EINTR) continue;
	    return -1;
	}
	if (r == 0) return 0;   // gestione chiusura socket
	left    -= r;
	bufptr  += r;
    }
    return size;
}

static inline int writen(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
	if ((r=write((int)fd ,bufptr,left)) == -1) {
	    if (errno == EINTR) continue;
	    return -1;
	}
	if (r == 0) return 0;
	left    -= r;
	bufptr  += r;
    }
    return 1;
}

#endif //UTILS_H
