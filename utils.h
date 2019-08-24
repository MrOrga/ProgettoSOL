

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

#define MAX_LENGHT 100
#define BUFF_SIZE 150

#define SERVER_NAME "[OBJSTORE]"
#define DATA "data"
#define MASK 0777
#define UNIX_PATH_MAX 108
#define SOCKNAME "objstore.sock"

#define CHECKNULL(s,e) \
    if (s==NULL) { perror(e);exit(errno); }
 int check_answer(char* answer,char * e)
{
    if(strncmp(answer,"OK",2)==0)
        return 1;
    else
        {
            fprintf(stderr,"%s : %s\n",e,answer);
            return 0;
        }

}
//funzione per essere sicuri di aver scritto tutti i byte
ssize_t writen(int fd ,const char*buff,size_t len)
{
    size_t byte_left=len;
    size_t byte_total=0;
    while(byte_total<len)
    {
        errno=0;
        size_t byte_writen=write(fd,buff,byte_left);
        if(byte_writen<0)
        {
            if(errno==EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        byte_total+=byte_writen;
        byte_left-=byte_writen;
    }
    return byte_total;
}
ssize_t readn(int fd ,const char*buff,size_t len)
{
    size_t byte_left=len;
    size_t byte_total=0;
    while(byte_total<len)
    {
        errno=0;
        size_t byte_readen=read(fd,buff,byte_left);
        if(byte_readen<0)
        {
            if(errno==EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        byte_total+=byte_readen;
        byte_left-=byte_readen;
    }
    return byte_total;
}
ssize_t read_to_new(int fd ,const char*buff,size_t len)
{
    size_t byte_left=len;
    size_t byte_total=0;
    int find=0;
    while((byte_total<len)&&(find==0))
    {
        errno=0;
        size_t byte_readen=read(fd,buff,byte_left);
        if(strchr(buff,'\n')!=NULL)
            find=1;

        if(byte_readen<0)
        {
            if(errno==EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        byte_total+=byte_readen;
        byte_left-=byte_readen;
    }
    return strlen(buff)+1;
}

#endif //UTILS_H
