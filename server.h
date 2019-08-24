

#ifndef SERVER_H
#define SERVER_H

#include "utils.h"

typedef struct server_
{
    ssize_t clients_connected;
    bool is_running;
    size_t size;

}server_;
server_ * server;
#endif //SERVER_H
