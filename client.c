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
#define string_test "pe9mlIcUkWoZU8JugWg8qSH4B5KSpehnuTmLoMvPzYaW7F5iRYvkKdr37tmmNxlcDperLHWo6Y9BFd3VxAfnbIXqlx8jvIPKKqqm"


void init_data()
{
    size_t len=strlen(string_test);
    data[0]=malloc(len*sizeof(char)+1);
    sprintf(data[0],"%s",string_test);
    for (int i = 1; i <N ; i++)
    {
        size_t len_blocco_i= len*i*(52.65);
        data[i]=malloc(len_blocco_i* sizeof(char)+1);
        memset(data[i], 0, len_blocco_i+1);
        for (int j = 0; j < len_blocco_i/len; j++)
        {
            //strcat(data[i],string_test);
            memcpy(data[i]+(j*strlen(string_test)),string_test,strlen(string_test));
        }
    }
}
void test1(char * name)
{
    init_data();
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
	    file[strlen(data[i])] = '\0';
            if(strcmp(data[i],file)==0)

	    {
                fprintf(stdout,"FILE %d RETRIEVED SUCCESS\n",i);
                op_success++;
	    }
            else
	    {
		fprintf(stdout,"FILE %d RETRIEVED FAILED\n",i);
                fprintf(stdout,"DATI RICEVUTI:\n%s\n",file );
		fprintf(stdout,"DATI INIZIALI:\n%s\n",data[i] );
                op_failed++;
	    }
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
        if(connect==0)
            exit(EXIT_FAILURE);
        int long op=strtol(argv[2],NULL,10);
        //char op=*argv[2];
	switch(op)
	{
	    case 1:
		test1(name);
		break;

	    case 2:
		test2(name);
		break;

	    case 3:
		test3(name);
		break;

	    default:
		fprintf(stderr,"THE TEST NUMBER IS WRONG,PLEASE CHECK AND TRY AGAIN\n");
		exit(EXIT_FAILURE);

	}
	fprintf(stdout,"-----OPERAZIONE EFFETTUATE CON SUCCESSO: %d,OPERAZIONE FALLITE:%d-----\n",op_success,op_failed);
        int disconnect=os_disconnect(name);
        if(disconnect!=0)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);

    }
}
