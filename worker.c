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
    pthread_t tid;
    if(pthread_create(&tid,NULL,worker_loop,new_worker)!=0)
        fprintf(stderr,"ERROR : phtread_create failed" );
    if(pthread_detach(tid)!=0)
        fprintf(stderr,"ERROR : phtread_detach failed" );

    //return new_worker;
}
void remove_worker(worker * current_worker)
{
    pthread_mutex_lock(&mutex);

    current_worker->is_logged=false;

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
    memset (message,0,BUFF_SIZE+1);
    while(server->is_running && current_worker->is_logged)
    {

	struct pollfd fds;
	fds.fd=current_worker->fd;
	fds.events=POLLIN;
	if(poll(&fds, 1, 10) >= 1)
	{
	    int byte_readen = read_to_new(current_worker->fd, message, BUFF_SIZE);
	    if (byte_readen <= 0)
	    {
		fprintf(stdout, "Client %s with fd %d is quiting\n", current_worker->name, current_worker->fd);
		break;
	    }
	    else
	    {
	        handler_msg(message, byte_readen, current_worker);
	    }
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
