
#ifndef HANDLERMESSAGE_H
#define HANDLERMESSAGE_H

#include "utils.h"
#include "worker.h"
#include "server.h"
size_t handler_msg(char* message,size_t len, worker* current_worker);
size_t handler_register(char *message,worker* current_worker);
size_t handler_store(char *message,size_t data_len,worker* current_worker);
size_t handler_retrieve(char *message,worker* current_worker);
size_t handler_delete(char *message,worker* current_worker);
size_t handler_leave(worker* current_worker);

#endif //PROGETTOSOL_HANDLERMESSAGE_H
