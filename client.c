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
#include "libobjstore.h"
#include <string.h>

#define N 20
#define MAX_NAME_LENGHT 100
#define INC_FILE_SIZE (999000/19) //costante per incremento lunghezza file
char *data[N];
int op_success=0,op_failed=0;
#define string_test "Apelle figlio di Apollo\n" \
"fece una palla di pelle di pollo;"

void init_data()
{
    size_t len=strlen(string_test);
    data[0]=malloc(len*sizeof(char));
    for (int i = 1; i <N ; ++i)
    {
        size_t len_blocco_i= len*i*(INC_FILE_SIZE);
        data[i]=malloc(len_blocco_i* sizeof(char)+1);
        for (int j = 0; j < len_blocco_i/len; ++j)
        {
            strcat(data[j],string_test);
        }
    }
}
void test1(char * name)
{
    init_data();
	fprintf(stdout,"\ncheck test1\n");
    for(int i=0;i<N;i++)
    {
        char file_name[MAX_NAME_LENGHT+1];
        sprintf(file_name,"%s-file-test-%d",name,i);
        size_t file_size=strlen(data[i]);

        if(os_store(file_name,data[i],file_size))
            op_success++;
        else
            op_failed++;
    }

    for(int i=0;i < N;i++)
    {
        free(data[i]);
    }
}
void test2(char *name)
{
    init_data();
    for (int i=0;i<N ;i++)
    {
        char file_name[MAX_NAME_LENGHT+1];
        sprintf(file_name,"%s-file-test-%d",name,i);
        char * file=NULL;
        if((file=os_retrieve(file_name))!=NULL)
        {
            if(strcmp(data[i],file)==0)
                op_success++;
            else
                op_failed++;
        }
        free(file);
    }
}
void test3(char * name)
{
    for(int i=0;i <N;i++)
    {
        char file_name[MAX_NAME_LENGHT+1];
        sprintf(file_name,"%s-file-test-%d",name,i);
        if(os_delete(file_name))
            op_success++;
        else
            op_failed++;
    }

}
int main(int argc,char * argv[])
{
    char *name;
    if(argc!=3)
    {
        fprintf(stderr,"wrong argument count,please check and try again");
        exit(EXIT_FAILURE);
    }
    else
    {
        name=argv[1];
        if(strlen(name)>255)
        {
            fprintf(stderr,"THE NAME IS TOO MUCH LONG,PLEASE CHECK AND TRY AGAIN");
            exit(EXIT_FAILURE);
        }
        //
        int connect=os_connect(name);
        if(connect!=0)
            exit(EXIT_FAILURE);
        int long op=strtol(argv[0],NULL,10);
        //char op=*argv[2];
        fprintf(stdout,"\ncheck point char%ld\n",op);
        if(op==1)
            test1(name);
        if(op==2)
            test2(name);
        if(op==3)
            test3(name);
        else
        {
            fprintf(stderr,"THE TEST NUMBER IS WRONG,PLEASE CHECK AND TRY AGAIN");
            exit(EXIT_FAILURE);
        }

        int disconnect=os_connect(name);
        if(disconnect!=0)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);

    }
}