#include "cgi_handle.h"


//SIGCHLD

void add_envp(pool_t *p, cgi_ev_t * cgiev, char *left, char *right)
{
	char *ptr;
	int len;

	len = strlen(left) + strlen(right) + 2;
	ptr = (char *) palloc(p, len);
	sprintf(ptr, "%s=%s", left, right);
	
	cgiev->ev[cgiev->count++] = ptr;
	
}

void cgi_handle(epoll_cgi_t *cgi_info, http_conf *g) {

	cgi_ev_t *cgi_ev;
	pool_t *p = cgi_info->p;
	char *argv[2] = {0};
	int infd[2], outfd[2];//infd server read, outfd server write;

	cgi_ev = (cgi_ev_t *)pcalloc(p, sizeof(cgi_ev_t));
	pipe(infd);
	pipe(outfd);

	cgi_ev->count = 0;
	add_envp(p, cgi_ev, "REQUEST_METHOD" , "execve");
	
	argv[0] = "/bin/sh";
	argv[1] = cgi_info->file->ptr;
	//argv[0] = (char *)"/www/aa.cgi";
	int pid = fork();
	
	switch(pid) {
		case -1: 
			break;
		case 0: 
			dup2(infd[1], STDOUT_FILENO);
			
			close(infd[0]);
			close(outfd[1]);
			if(execve(argv[0], argv, environ) == 0) {
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
	

			cgi_info->fd = infd[0];
			cgi_info->pid = pid;
			cgi_info->last_run_ts = time((time_t*)NULL);
			cgi_info->p = p;
			cgi_info->status = CGI_STATUS_RUN;
			
			
			epoll_extra_data_t * extra;
			extra = (epoll_extra_data_t *) pcalloc(p, sizeof(epoll_extra_data_t));
			extra->type = CGIFD;
			extra->ptr = (void *) cgi_info;

			epoll_add_fd(g->epfd, infd[0], EPOLL_R, (void *)extra);	
			break;
	}


}


int init_cgi_data_struct(string *execute_file, execute_cgi_info_manager_t *cgi_info_manager, unsigned int ts) {
	pool_t *p = pool_create();
	epoll_cgi_t * cgiInfo = (epoll_cgi_t *)palloc(p, sizeof(epoll_cgi_t ));
	cgiInfo->file = string_init_from_str(p, execute_file->ptr, execute_file->len);
	cgiInfo->last_add_ts = ts;
	cgiInfo->p = p;
	hash_add_ptr(cgi_info_manager->p, cgi_info_manager->h, cgiInfo->file->ptr, cgiInfo->file->len, cgiInfo);
	return 0;
}

int get_cgi_operator_handle(epoll_cgi_t *cgi_info) {
	int size = 1024;
	int isEnd = 1;
	int count ;

	if(cgi_info->out == NULL) {
		cgi_info->out =  list_buffer_create(cgi_info->p, size);
	}
	list_buffer_item_t *foot_item = cgi_info->out->foot;
	if(size <= foot_item->used) {
		list_buffer_add(cgi_info->p, cgi_info->out);
		foot_item = cgi_info->out->foot;
	}
	
	printf("isEnd ----\n");

	 while((count = read(cgi_info->fd, foot_item->ptr, (size - foot_item->used))) > 0) {
		 isEnd = 0;
		 foot_item->used += count;
		 if(size <= foot_item->used) {
			list_buffer_add(cgi_info->p, cgi_info->out);
			foot_item = cgi_info->out->foot;
		 }
    }

	printf("isEnd \n");
	if(isEnd) {
		if( cgi_info->status == CGI_STATUS_RUN) {
			cgi_info->status = CGI_STATUS_CLOSEING;
			kill(cgi_info->pid, SIGKILL);
			close(cgi_info->fd);
			pool_destroy(cgi_info->p);
			cgi_info->p = NULL;
			cgi_info->status = CGI_STATUS_END;
		}
		
	}
}


