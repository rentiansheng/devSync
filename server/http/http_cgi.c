#include "http_cgi.h"


void add_envp(pool_t *p, cgi_ev_t * cgiev, char *left, char *right)
{
	char *ptr;
	int len;

	len = strlen(left) + strlen(right) + 2;
	ptr = (char *) palloc(p, len);
	sprintf(ptr, "%s=%s", left, right);
	
	cgiev->ev[cgiev->count++] = ptr;
	
}

static void add_handle(string *exce_file, exce_cgi_info_manager_t *cgi_info) {

	pool_t *p = (pool_t *)pool_create();//单个cgi处理使用内存池
	char *fileName = (char *)pcalloc(p, sizeof(char)*(file->len+1));

	cgi_ev_t *cgi;
	char *argv[2] = {0};
	int infd[2], outfd[2];//infd server read, outfd server write;

	cgi = (cgi_ev_t *)pcalloc(p, sizeof(cgi_ev_t));
	pipe(infd);
	pipe(outfd);

	//不是使用file，应为file会在稍后回收，
	strncpy(fileName, file->ptr, file->len);
	cgi->count = 0;
	if(con->in->http_method == _GET) {
		add_envp(con->p, cgi, "REQUEST_METHOD" , "GET");
	}else if(con->in->http_method == _POST){
		add_envp(con->p, cgi, "REQUEST_METHOD" , "POST");
	}else if(con->in->http_method == _PUT){
		add_envp(con->p, cgi, "REQUEST_METHOD" , "PUT");
	}else {
		add_envp(con->p, cgi, "REQUEST_METHOD" , "NONE");
	}
	
	argv[0] = fileName;
	//argv[0] = (char *)"/www/aa.cgi";
	int pid = fork();
	
	switch(pid) {
		case -1: 
			break;
		case 0: 
			dup2(infd[1], STDOUT_FILENO);
			
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

			hash_item_t * hitem  = get_hash_item(cgi_info->h, fileName, strlen(fileName));
			epoll_cgi_t *cgi_extra ;
			if(hitem) {
				cgi_extra = (epoll_cgi * )hitem->value.ptr;
				cgi_data->fd = infd[0];
				cgi_data->pid = pid;
				cgi_data->last_active_ts = time((time_t*)NULL);
				cgi_extra->p = p;
				cgi_extra->status = CGI_STATUS_RUN;
			} else {
				/*cgi 管理信息需要长时间存在，不能保存在单个cgi处理的内存池中*/
				cgi_extra = (epoll_cgi_t *) pcalloc(cgi_info->p, sizeof(epoll_cgi_t));
				cgi_extra->file = string_init_from_str(cgi_info->p, fileName);
				cgi_extra->fd = infd[0];
				cgi_extra->pid = pid;
				cgi_extra->last_active_ts = time((time_t*)NULL);
				cgi_extra->p = p;
				cgi_extra->status = CGI_STATUS_RUN;
				hash_add_ptr(cgi_info->h, cgi_extra->file->ptr, cgi_extra->file->len, cgi_extra);

			}
			
			
			epoll_extra_data_t * extra;
			extra = (epoll_extra_data_t *) pcalloc(p, sizeof(epoll_extra_data_t));
			extra->type = CGIFD;
			extra->ptr = (void *) cgi_extra;

			make_fd_non_blocking(infd[0]);
			epoll_add_fd(g->epfd, infd[0], EPOLL_R, (void *)extra);
			
			break;
	}


}



int add_cgi_handle(string *exce_file, exce_cgi_info_manager_t *cgi_info_manager)
{
	if(exce_file && exce_file->len && exce_file->ptr != NULL) {
		hash_item_t * hitem  = get_hash_item(cgi_info_manager->h, exce_file->ptr, exce_file->len);
		epoll_cgi_t  *cgiInfo ;
		if(hitem) {
			cgiInfo = (epoll_cgi_t *)hitem->value.ptr;
			if( cgiInfo->status == CGI_STATUS_RUN) {
				cgiInfo->status = CGI_STATUS_CLOSEING;
				kill(cgiInfo->pid, SIGKILL);
				close(cgiInfo->fd);
				pool_destroy(cgiInfo->p);
				cgiInfo->p = NULL;
				add_handle(exce_file, cgi_info_manager);
			} else if ( cgiInfo->status == CGI_STATUS_END) {
				add_handle(exce_file, cgi_info_manager);
			}
		}else  {
			add_handle(exce_file, cgi_info_manager);
		}
	} 
	
	return 0;	
}

int get_cgi_operator_handle(epoll_cgi_t *cgi_info, exce_cgi_info_manager_t *cgi_info_manager) {
	int size = 1024;
	int isEnd = 1;
	if(cgi_info->out == NULL) {
		cgi_info->out =  list_buffer_create(cgi_info->p, size);
	}
	list_buffer_item_t *foot_item = cgi_info->out->foot;
	if(size <= foot_item->used) {
		list_buffer_add(cgi_info->p, cgi_info->out);
		foot_item = cgi_info->out->foot;
	}
	


	 while((count = read(con->fd, foot_item->ptr, (size - foot_item->used))) > 0) {
		 isEnd = 0;
		 foot_item->used += count;
		 if(size <= foot_item->used) {
			list_buffer_add(cgi_info->p, cgi_info->out);
			foot_item = cgi_info->out->foot;
		 }
    }

	if(isEnd) {
		hash_item_t * hitem  = get_hash_item(cgi_info_manager->h, cgi_info->file->ptr, cgi_info->file->len);
		epoll_cgi_t  *cgiInfo ;
		if(hitem) {
			cgiInfo = (epoll_cgi_t *)hitem->value.ptr;
			if( cgiInfo->status == CGI_STATUS_RUN) {
				cgiInfo->status = CGI_STATUS_CLOSEING;
				kill(cgiInfo->pid, SIGKILL);
				close(cgiInfo->fd);
				pool_destroy(cgiInfo->p);
				cgiInfo->p = NULL;
				cgiInfo->status = CGI_STATUS_END;
			}
		}
	}
}

exce_cgi_info_manager_t * initCgiManager() {
	pool_t * cgi_pool = (pool_t *)pool_create();
	hash *h = hash_init(cgi_pool);
	exce_cgi_info_manager_t *exce_cgi_info 
		= (exce_cgi_info_manager_t *)pcalloc(cgi_pool, sizoef(exce_cgi_info_manager_t));
	exce_cgi_info->p = cgi_pool;
	exce_cgi_info->h = h;

	return exce_cgi_info;
}
