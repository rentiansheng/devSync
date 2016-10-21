#include "cgi.h"


void
add_envp(pool_t *p, cgi_ev_t * cgiev, char *left, char *right)
{
	char *ptr;
	int len;

	len = strlen(left) + strlen(right) + 2;
	ptr = (char *) palloc(p, len);
	sprintf(ptr, "%s=%s", left, right);
	
	cgiev->ev[cgiev->count++] = ptr;
	
}



int
cgi_handle(http_conf *g, http_connect_t *con)
{
	con->next_handle = NULL;

	cgi_ev_t *cgi;
	char *argv[2] = {0};
	int infd[2], outfd[2];//infd server read, outfd server write;

	cgi = (cgi_ev_t *)pcalloc(con->p, sizeof(cgi_ev_t));
	pipe(infd);
	pipe(outfd);

	cgi->count = 0;
	if(con->in->http_method == _GET) {
		add_envp(con->p, cgi, "REQUEST_METHOD" , "GET");
		char *query_string = (char *)pcalloc(con->p, con->in->args->size+1);
		strncpy(query_string, con->in->args->ptr, con->in->args->size);
		add_envp(con->p, cgi, "QUERY_STRING", query_string);
	}else {
		add_envp(con->p, cgi, "REQUEST_METHOD" , "POST");
		char *str_len = (char *) pcalloc(con->p, con->in->content_length->size+1);
		strncpy(str_len, con->in->content_length->ptr, con->in->content_length->size);
		add_envp(con->p, cgi, "CONTENT_TYPE", str_len);
	}
	
	argv[0] = con->out->physical_path->ptr +1;
	//argv[0] = (char *)"/www/aa.cgi";
	
	switch(fork()) {
		case -1: 
			con->out->status_code = HTTP_BAD_GATEWAY;
			break;
		case 0: 
			dup2(infd[1], STDOUT_FILENO);
			dup2(con->fd, STDIN_FILENO);
			
			close(infd[0]);
			close(outfd[1]);
			if(execve(argv[0], argv, cgi->ev) == 0) {
			}
			else {
			}

			close(infd[1]);
			close(outfd[0]);
			exit(0);
			break;
		default:
			close(infd[1]);
			close(outfd[0]);

			//write(outfd[1], send, count);
			close(outfd[1]);
			epoll_cgi_t *cgi_extra ;
			epoll_extra_data_t * extra;

			extra = (epoll_extra_data_t *) pcalloc(con->p, sizeof(epoll_extra_data_t));
			cgi_extra = (epoll_cgi_t *) pcalloc(con->p, sizeof(epoll_cgi_t));

			cgi_extra->con = con;
			cgi_extra->fd = infd[0];

			extra->type = CGIFD;
			extra->ptr = (void *) cgi_extra;
			
			epoll_add_fd(g->epfd, infd[0], EPOLL_R, (void *)extra);
			break;
	}


	return 0;	
}
