#include "utils.h"
#include "libobjstore.h"
static int fd_skt=-1;// socket per la connessione inizializzato a -1 per controllare se esiste fd prima di un op
int os_connect(char *name)
{
    CHECKNULL(name,"invalid argument");
    if(fd_skt!=-1)
    {
        perror("Socket still open");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un sa; // indirizzo AF_UNIX
    memset (&sa,0,sizeof(sa));
    strncpy(sa.sun_path, SOCKNAME,UNIX_PATH_MAX);
    sa.sun_family = AF_UNIX;
    fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd_skt==-1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    while (connect(fd_skt,(struct sockaddr*)&sa,sizeof(sa)) == -1 )
    {
        if ( errno == ENOENT )
            sleep(1); /* sock non esiste */
        else
            return 0;
    }
    //creazione buffer ed invio richiesta al server
    char buff[BUFF_SIZE];
    sprintf(buff,"%s %s \n","REGISTER",name);
    int err_writen=writen(fd_skt,buff,strlen(buff));
    if(err_writen==-1)
    {
        perror("writen os_connect");
        exit(EXIT_FAILURE);
    }
    //risposta server
    char server_response[BUFF_SIZE];
    int err_read=read_to_new(fd_skt,server_response,BUFF_SIZE);
    if(err_read>0)
        return check_answer(server_response,"Connection failed");
    else
    {
    	perror("ERROR ON READ os_connect");
    	return -1;
    }



}

int os_store(char *name, void *block, size_t len)
{
    CHECKNULL(name,"Invalid argument");
    CHECKNULL(block,"Invalid argument");
    if(fd_skt==-1)
    {
        perror("Socket not opened");
        exit(EXIT_FAILURE);
    }
    //creazione e invio messaggio
    char* message=malloc((BUFF_SIZE+len+1)* sizeof(char));
    sprintf(message,"%s %s %ld \n %s","STORE",name,len,(char*)block);
    //fprintf(stdout,"%s %s %ld \n %s\n","STORE",name,len,(char*)block);
    size_t len_msg=strlen(message);
    int err_writen=writen(fd_skt,message,len_msg);
    if(err_writen==-1)
    {
        perror("writen os_store");
        exit(EXIT_FAILURE);
    }
    //risposta server
    char server_response[BUFF_SIZE];
    int err_read=read_to_new(fd_skt,server_response,BUFF_SIZE);
    free(message);
    if(err_read>0)
    {
	return check_answer(server_response,"Store failed");
    }
    else
    {
	    perror("ERROR ON READ os_store");
	    return -1;
    }


}

void * os_retrieve(char *name)
{
    char * data=NULL;
    CHECKNULL(name,"invalid argument");
    char buff[BUFF_SIZE];
    sprintf(buff,"%s %s \n","RETRIEVE",name);
    int err_writen=writen(fd_skt,buff,strlen(buff));
    if(err_writen==-1)
    {
        perror("writen os_RETRIEVE");
        exit(EXIT_FAILURE);
    }
    //risposta server
    char server_response[BUFF_SIZE+1];
    //memset(server_response,0,BUFF_SIZE+1);
    int byte_read=read_to_new(fd_skt,server_response,BUFF_SIZE);
    //fprintf(stdout,"\n byte letti:%d\n",byte_read);
    char* partial_data=NULL;
    char* check_ans=NULL;
    check_ans=strtok_r(server_response," ",&partial_data);
    if(strcmp(check_ans,"DATA")==0)
    {
        char *rest;
        //separo lunghezza
        char *len_c=strtok_r(partial_data," ",&partial_data);
        //conversione
        long int len = strtol(len_c, &rest, 10);
        //separo dati da \n
	partial_data=strtok_r(partial_data,"\n",&partial_data);
        partial_data=strtok_r(partial_data," ",&partial_data);
        //fprintf(stdout,"\nlen=%ld data part1=%s\n",len,partial_data);

        //strcat(data,partial_data);
	//lunghezza dei dati da leggere +4 perchè 3 spazi e /n

	ssize_t partial_size=(byte_read-(strlen("DATA")+strlen(len_c)+4));
	ssize_t readlen=len-partial_size;
	data=(char*)malloc((len+1)* sizeof(char));
        memcpy(data,partial_data,sizeof(char)*partial_size);
	//fprintf(stdout,"\nlen=%ld len data part=%ld\n",len,partial_size);
        //sprintf(data,"%s",partial_data);
	if(readlen>0)
	{
	    char* partial_data2=(char*)malloc((readlen+1)* sizeof(char));
	    byte_read=readn(fd_skt,partial_data2,readlen);
	    //strcat(data,partial_data2);
	    memcpy((data+(sizeof(char)*partial_size)),partial_data2, sizeof(char)*readlen);
	    free(partial_data2);
	}
    }
    else
    {
        fprintf(stderr,"%s : %s\n","os_RETRIEVE",server_response);
        return NULL;
    }
	return data;
}

int os_delete(char *name)
{

    CHECKNULL(name,"Invalid argument");
    if(fd_skt==-1)
    {
        perror("Socket not opened");
        exit(EXIT_FAILURE);
    }
    //creazione e invio messaggio
    char message[BUFF_SIZE];
    sprintf(message, "%s %s \n", "DELETE", name);
    int err_writen=writen(fd_skt,message,strlen(message));
    if(err_writen==-1)
    {
        perror("writen os_delete");
        exit(EXIT_FAILURE);
    }
    //risposta server
    char server_response[BUFF_SIZE];
    int err_read=read_to_new(fd_skt,server_response,BUFF_SIZE);
    if(err_read>0)
    	return check_answer(server_response,"Delete failed");
    else
    {
    	perror("ERROR READ os_delete");
    	return -1;
    }

}

int os_disconnect()
{
    char buff[BUFF_SIZE];
    sprintf(buff,"%s \n","LEAVE");
    int err_writen=writen(fd_skt,buff,strlen(buff));
    if(err_writen==-1)
    {
        perror("writen os_disconnect");
        exit(EXIT_FAILURE);
    }
    //risposta server
    char server_response[BUFF_SIZE];
    int err_read=read_to_new(fd_skt,server_response,BUFF_SIZE);
    close(fd_skt);
    fd_skt=-1;
    if(err_read>0)
    	return check_answer(server_response,"Disconnection failed");
    else
    {
    	perror("ERROR READ os_disconnect");
    	return -1;
    }
}