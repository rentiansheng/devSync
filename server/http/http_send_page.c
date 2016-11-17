/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "http_send_page.h"

int send_put_result(http_connect_t * con)
{
	response *out = con->out;
	pool_t *p = con->p;
	char *msg =  (char *) palloc(p, sizeof(char)*1024);
	sprintf(msg, "HTTP/1.1 %d  OK\r\nConnection: Close\r\nContent-Type: text/html\r\n", con->out->status_code);
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);

	if(out != NULL && out->status_code == HTTP_OK) {
		sprintf(msg, "%s\r\n\success\r\n\r\n", msg);
	} else {
		sprintf(msg, "%s\r\nerror %s\r\n\r\n", msg, strerror(errno));
	}

   write (con->fd, msg, strlen(msg));
  

   con->next_handle = NULL;
   return -1;
}


int send_without_execute_result(http_connect_t * con)
{
	response *out = con->out;
	pool_t *p = con->p;
	char *msg =  (char *) palloc(p, sizeof(char)*1024);
	sprintf(msg, "HTTP/1.1 200  OK\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);

	if(out != NULL && out->status_code == HTTP_OK) {
		sprintf(msg, "%s\r\nsuccess\r\n\r\n", msg);
	} else {
		sprintf(msg, "%s\r\nerror\r\n\r\n", msg);
	}
	strcpy(msg, "The most recent compilation result was not found, and the value of the most recently compiled result was saved for a maximum of one days");

   write (con->fd, msg, strlen(msg));

   con->next_handle = NULL;
   return -1;
}

int send_put_forbidden_result(http_connect_t * con)
{
	//response *out = con->out;
	pool_t *p = con->p;
	char *msg =  (char *) palloc(p, sizeof(char)*1024);
	sprintf(msg, "HTTP/1.1 403  Forbidden\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);

	sprintf(msg, "%s\r\n403 Forbidden\r\n\r\n", msg);


   write (con->fd, msg, strlen(msg));

   con->next_handle = NULL;
   return -1;
}

int send_execute(http_connect_t * con) {
	pool_t *p = con->p;
	struct stat buf;
	int i = 0;
	int nameSize = con->in->uri->len+30;
	char *fileName =(char *)palloc(p,nameSize); //存储编译输出内容的文件
	char *name =(char *)palloc(p,con->in->uri->len+1); //存储编译输出内容的文件

	memcpy(name, con->in->uri->ptr, con->in->uri->len*sizeof(char));

	sprintf(fileName, "/tmp/devsync%s.out", name);//ts);

	//跳过/tmp/devsync
	for(i = 12 ; i < strlen(fileName); i++) {
		if(fileName[i] == '/') {
			fileName[i] = '-';
		}
	}


	if(stat(fileName, &buf) != 0 ) {
		return send_without_execute_result(con);
    } else {

		char *msg =  (char *) palloc(p, sizeof(char)*1024);
		sprintf(msg, "HTTP/1.1 200  OK\r\nConnection: Close\r\nContent-Type: text/plain\r\n");
		if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);
		sprintf(msg, "%s\r\n", msg);

		write (con->fd, msg, strlen(msg));
		
		FILE *fp = fopen(fileName, "r");
		con->write_file.fp = fp;
        con->write_file.len = 0;


		sendfile(con->fd, fileno(fp), 0,buf.st_size);
	
	}

	con->next_handle = NULL;
	return -1;
}


