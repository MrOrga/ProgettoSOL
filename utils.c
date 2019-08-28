

#include "utils.h"
int check_answer(char* answer,char * e)
{
	//fprintf(stdout,"\ncheck point %s\n",answer);
    if(strncmp(answer,"OK",2)==0)
        return 1;
    else
    {
        fprintf(stderr,"%s : %s\n",e,answer);
        return 0;
    }

}
ssize_t writen(int fd ,void *buff,size_t len)
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
ssize_t readn(int fd ,void *buff,size_t len)
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
ssize_t read_to_new(int fd ,void*buff,size_t len)
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
    return byte_total;
}