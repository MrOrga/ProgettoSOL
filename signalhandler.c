
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
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGUSR1);
    while (server->is_running)
    {
	sigprocmask(SIG_BLOCK, &set, NULL);
	int sig;
	sigwait(&set, &sig);
	switch(sig) {
	    case SIGINT:
	        fprintf(stdout,"\nSIGINT RECEIVED\n");
		fprintf(stdout,"Server shutdown\n");
		server->is_running = false;
		break;
	    case SIGTERM:
		fprintf(stdout,"\nSIGTERM RECEIVED\n");
		fprintf(stdout,"Server shutdown\n");
		server->is_running = false;
		break;
	    case SIGUSR1:
		fprintf(stdout,"\nSIGUSR1 RECEIVED\n");
		server_info();
		break;
	}
    }

    pthread_exit(NULL);
}
