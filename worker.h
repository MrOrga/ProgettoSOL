

#ifndef WORKER_H
#define WORKER_H

#include "utils.h"

typedef struct worker
{
    int fd;
    struct worker * next;
    struct worker * prev;
    //pthread_t tid;
    char name[MAX_LENGHT+1];
    bool is_logged;
    bool is_registered;
}worker;

void create_worker(int fd);
void * worker_loop(void *args);
bool search_user(char *name);
void remove_worker(worker * current_worker);
//void stop_worker(worker * current_worker);

#endif //WORKER_H
