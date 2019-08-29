#include "worker.h"
#include "server.h"
#include "handlermessage.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
worker * workers_list=NULL;

void create_worker(int fd)
{
    //acquisizione mutex per lavorare sulla lista di worker
    pthread_mutex_lock(&mutex);
    //inizializzazione struttura worker
    worker * new_worker=(worker*)malloc(sizeof(worker));
    new_worker->fd=fd;
    new_worker->prev=NULL;
    new_worker->next=NULL;
    new_worker->is_logged=true;
    new_worker->is_registered=false;
    memset(new_worker->name,0,MAX_LENGHT);
    //inserimento lista
    if(workers_list == NULL)
    {
	workers_list = new_worker;
    }
    else
	{
	workers_list->prev = new_worker;
	new_worker->next = workers_list;
	workers_list=new_worker;	
	}

    pthread_mutex_unlock(&mutex);
    if(pthread_create(&new_worker->tid,NULL,worker_loop,new_worker)!=0)
        fprintf(stderr,"ERROR : phtread_create failed" );
    if(pthread_detach(new_worker->tid)!=0)
        fprintf(stderr,"ERROR : phtread_detach failed" );

    //return new_worker;
}
void remove_worker(worker * current_worker)
{
/*    pthread_mutex_lock(&mutex);
    server->clients_connected--;

    if(current_worker->next != NULL)
    {
	if(workers_list == current_worker)
	{
	    workers_list = current_worker->next;
	}
	else
        	current_worker->next->prev = current_worker->prev;
    }

    if(current_worker->prev != NULL)
    {
	    current_worker->prev->next = current_worker->next;
    }

    pthread_mutex_unlock(&mutex);
    free(current_worker);*/
    pthread_mutex_lock(&mutex);

    current_worker->is_logged=false;
    server->clients_connected--;

    if(current_worker->prev== NULL)
    {
	if (current_worker->next != NULL)
	    current_worker->next->prev=NULL;
	workers_list = current_worker->next;
    }
    else
    {
	if (current_worker->next != NULL)
	    current_worker->next->prev=current_worker->prev;
	current_worker->prev->next=current_worker->next;

    }
    pthread_mutex_unlock(&mutex);

}
void * worker_loop(void *args)
{
    worker* current_worker=(worker*) args;
    char message[BUFF_SIZE+1];
    while(server->is_running && current_worker->is_logged)
    {
        memset (message,0,sizeof(message));
        int byte_readen = read_to_new(current_worker->fd, message,BUFF_SIZE);
        if(byte_readen <= 0)
        {
            fprintf(stdout, "Client %s with fd %d is quiting\n", current_worker->name, current_worker->fd);
            break;
        }
        else
        {
            handler_msg(message,byte_readen, current_worker);
        }
    }

    close(current_worker->fd);//aggiungere controllo
    remove_worker(current_worker);
    free(current_worker);
    pthread_exit(NULL);
}
bool search_user(char *name)
{
    bool find=false;
    pthread_mutex_lock(&mutex);
    worker *current = workers_list;
    while (current != NULL)
    {
        if (strcmp(current->name,name) == 0)
            return true;
        current=current->next;
    }
    pthread_mutex_unlock(&mutex);
    return find;
}
