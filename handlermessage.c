
#include <fcntl.h>
#include "handlermessage.h"

size_t send_KO(char* err,worker * current_worker)
{
    char ko[MAX_LENGHT+1] ;
    sprintf(ko,"%s %s \n","KO",err);
    fprintf(stderr,"%s %s \n","KO",err);
    return writen(current_worker->fd,ko,strlen(ko));
}
size_t send_OK(worker* current_worker)
{
    char *ok="OK \n";
    return writen(current_worker->fd,ok,strlen(ok));

}
size_t handler_msg(char* message,size_t len, worker* current_worker)
{
    CHECKNULL(message,"invalid argument")
    CHECKNULL(current_worker,"invalid worker")
    char* rest=NULL;
    char* op=strtok_r(message," " ,&rest);
    CHECKNULL(op,"invalid argument")

    if (strcmp(op,"REGISTER")==0)
    {
        return handler_register(rest,current_worker);
    }
    if (strcmp(op,"STORE")==0)
    {
        return handler_store(rest,len,current_worker);
    }
    if (strcmp(op,"RETRIEVE")==0)
    {
        return handler_retrieve(rest,current_worker);
    }
    if (strcmp(op,"DELETE")==0)
    {
        return handler_delete(rest,current_worker);
    }
    if (strcmp(op,"LEAVE")==0)
    {
        return handler_leave(current_worker);
    }

    fprintf(stderr,"COMMAND NOT FOUND,CHECK IT AND TRY AGAIN" );
    return -1;
}
size_t handler_register(char *message ,worker *current_worker)
{
    char* rest=NULL;
    char* name=strtok_r(message," " ,&rest);
    if(search_user(name)==true)
        return send_KO("REGISTER FAILED NAME ALREADY TAKEN",current_worker);
    if(current_worker->is_registered==true)
        return send_KO("REGISTER FAILED ALREADY LOGGED",current_worker);
    char path[UNIX_PATH_MAX];
    //set user name
    sprintf(current_worker->name,"%s",name);
    sprintf(path,"%s/%s",DATA,name);
    mkdir(path,MASK);
    current_worker->is_registered=true;
    fprintf (stdout,"OBJECT STORE:---Client %s fd:%d REGISTER SUCCESS ---\n",current_worker->name,current_worker->fd);
    return send_OK(current_worker);

}
size_t handler_store(char *message,size_t data_len,worker* current_worker)
{
    char* data=NULL;
    char* rest=NULL;
    char* partial_data=NULL;
    char* name=strtok_r(message," " ,&partial_data);
    //separo lunghezza
    char *len_c=strtok_r(partial_data," ",&partial_data);
    //conversione
    long int len = strtol(len_c, &rest, 10);
    //separo dati da \n
    partial_data=strtok_r(partial_data,"\n" ,&partial_data);
    partial_data=strtok_r(partial_data," " ,&partial_data);

    data=(char*)malloc((len+1)* sizeof(char));

    ssize_t partial_size=(data_len-(strlen("STORE")+strlen(name)+strlen(len_c)+5));
    ssize_t readlen=len-(partial_size);
    memcpy(data,partial_data, sizeof(char)*partial_size);

    if(readlen>0)
    {
	    char* partial_data2=(char*)malloc((readlen+1)* sizeof(char));
	    data_len=readn(current_worker->fd,partial_data2,readlen);
	    memcpy((data+(sizeof(char)*partial_size)),partial_data2, sizeof(char)*readlen);
	    free(partial_data2);
    }
    char path[UNIX_PATH_MAX];
    sprintf(path, "%s/%s/%s", DATA, current_worker->name,name);

    FILE* file = fopen(path, "w");

    CHECKNULL(file,"ERROR FOPEN STORE")

    ssize_t byte_written = 0;
    while(byte_written < len)
    {
        size_t now = fwrite(data+byte_written, sizeof(char), len-byte_written, file);//aggiungere byte_readen??
        byte_written += now;
    }
    free(data);
    fclose(file);
    if(byte_written<len)
        send_KO("ERROR STORE worker",current_worker);
    fprintf (stdout,"OBJECT STORE:---Client %s fd:%d STORE SUCCESS -------\n",current_worker->name,current_worker->fd);
    return send_OK(current_worker);


}
size_t handler_retrieve(char *message,worker* current_worker)
{
    char* partial_data=NULL;
    char* name=strtok_r(message," " ,&partial_data);
    char path[UNIX_PATH_MAX];
    sprintf(path, "%s/%s/%s", DATA, current_worker->name,name);
    off_t byte_readen=0;
    off_t len=1;//dichiaro len a 1 perchè dopo vado a controllare byte_readen <len
    FILE* file = fopen(path, "r");
    if (file != NULL)
    {
	struct stat info;
	if(stat(path, &info) != 0)
	    perror("stat in read_from_disk failed");
	len = info.st_size;
	char data[len];
	memset(data,0,len);

	while(byte_readen < len)
	{
	    size_t now = fread(data + byte_readen, sizeof(char), len - byte_readen, file);
	    byte_readen += now;
	}
	    if(byte_readen<len)
		return send_KO("ERROR STORE worker",current_worker);
	    else
	    {

		fclose(file);
		char header[BUFF_SIZE+1];
		memset(header,0,BUFF_SIZE+1);
		sprintf(header,"DATA %ld \n ",len);
		char* message_full=(char*)malloc((strlen(header)+len+1)* sizeof(char));
		memcpy(message_full,header,strlen(header)* sizeof(char));
		memcpy(message_full+(strlen(header)* sizeof(char)),data,len* sizeof(char));
		message_full[strlen(header)+len]='\0';
		size_t byte_writen=writen(current_worker->fd,message_full,strlen(header)+len);

		if(byte_writen<0)
		{
		    free(message);
		    perror("Failed write retrieve worker");
		    return byte_writen;
		}
		free(message_full);
		fprintf (stdout,"OBJECT STORE:---Client %s fd:%d RETRIEVE SUCCESS ---\n",current_worker->name,current_worker->fd);

		return byte_writen;
		}

    }
    else
    {
        perror("Failed opening file");
	return -1;
    }
}

size_t handler_delete(char *message,worker* current_worker)
{
    char* partial_data=NULL;
    char* name=strtok_r(message," " ,&partial_data);
    char path[UNIX_PATH_MAX];
    sprintf(path, "%s/%s/%s", DATA, current_worker->name,name);
    if(unlink(path)!=0)
    {
        return send_KO("ERROR DELETE FILE",current_worker);
    }
    fprintf (stdout,"OBJECT STORE:---Client %s fd:%d DELETE SUCCESS ---\n",current_worker->name,current_worker->fd);

    return send_OK(current_worker);
}
size_t handler_leave(worker* current_worker)
{
    current_worker->is_logged=false;
    fprintf (stdout,"OBJECT STORE:---Client %s fd:%d LEAVE SUCCESS ------\n",current_worker->name,current_worker->fd);
    return send_OK(current_worker);

}