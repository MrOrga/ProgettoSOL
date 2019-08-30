
#include "signalhandler.h"
void create_signal_handler()
{
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);
    pthread_t tid;
    if(pthread_create(&tid,NULL,signal_loop,NULL)!=0)
	fprintf(stderr,"ERROR : phtread_create failed" );
    if(pthread_detach(tid)!=0)
	fprintf(stderr,"ERROR : phtread_detach failed" );
}
void * signal_loop()
{
    sigset_t set;
    sigemptyset(&set);
    while (server->is_running) {
	sigprocmask(SIG_BLOCK, &set, NULL);
	int sig;
	sigwait(&set, &sig);
	switch(sig) {
	    case SIGINT:
	        fprintf(stdout,"SIGINT RICEVUTA");
		server->is_running = false;
		//close(server->fd);
		break;
	    case SIGTERM:
		fprintf(stdout,"SIGTERM RICEVUTA");
		fprintf(stdout,"Server shutdown\n");
		server->is_running = false;
		//close(server->fd);
		break;
	    case SIGUSR1:
		fprintf(stdout,"SIGUSR1 RICEVUTA");
		//server_info();da aggiungere
		//sigaddset(&set, SIGINT);
		//sigaddset(&set, SIGTERM);
		//sigaddset(&set, SIGUSR1);
		break;
	}
    }

    pthread_exit(NULL);
}
