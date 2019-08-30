

#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "worker.h"
#include "signalhandler.h"

typedef struct server_
{
    int fd;
    volatile sig_atomic_t clients_connected;
    volatile sig_atomic_t is_running;
    long double size;
    int n_obj;

}server_;
server_ * server;
void server_info();
#endif //SERVER_H
