
#ifndef LIBOBJSTORE_H
#define LIBOBJSTORE_H

int os_connect(char *name);

int os_store(char *name, void *block, size_t len);

void *os_retrieve(char *name);

int os_delete(char *name);

int os_disconnect();

#endif //LIBOBJSTORE_H
