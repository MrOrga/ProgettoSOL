
#include "utils.h"
#include "server.h"
#include "worker.h"


int main()
{
    server = (server_ *) malloc(sizeof(server_));
    server->is_running=false;
    server->clients_connected=0;

    int fd_skt;// socket per la connessione
    struct sockaddr_un sa; // indirizzo AF_UNIX
    memset(&sa, 0, sizeof(sa));
    strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
    sa.sun_family = AF_UNIX;
    //creazione directory con permessi
    mkdir(DATA, MASK);
    //pulizia socket precedente e apertura nuovo socket
    unlink(SOCKNAME);
    fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_skt == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    //creazione del bind
    int bind_err = bind(fd_skt, (struct sockaddr *) &sa, sizeof(sa));
    if (bind_err == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    //metto il server in ascolto
    int listen_err = listen(fd_skt, SOMAXCONN);
    if (listen_err == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    server->is_running=true;
    while (server->is_running)
    {
        /*
         * ricezione e creazione thread per client
         */
        int client_fd = accept(fd_skt, (struct sockaddr *) NULL, NULL);
        if (client_fd > 0)
        {
                create_worker(client_fd);//creazione_worker
        }
        if (client_fd == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }


    }
    close(fd_skt);

    return 0;
}
