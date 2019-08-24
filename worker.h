

#ifndef WORKER_H
#define WORKER_H


#include <sys/types.h>
#include "utils.h"

typedef struct worker
{
    int fd;
    struct worker * next;
    struct worker * prev;
    pthread_t tid;
    char * name;
    bool is_logged;
    bool is_registered;
}worker;

worker create_worker(int fd);
void * worker_loop(void *args);
bool search_user(char *name);
void remove_worker(worker * current_worker);

#endif //WORKER_H
