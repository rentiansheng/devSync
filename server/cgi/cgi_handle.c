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
	pool_t *p  = pool_create();
	char *argv[3] = {0};
	int infd[2], outfd[2];//infd server read, outfd server write;

	cgi_ev = (cgi_ev_t *)pcalloc(p, sizeof(cgi_ev_t));

	cgi_info->p = p;
	pipe(infd);
	pipe(outfd);

	cgi_ev->count = 0;
	add_envp(p, cgi_ev, "REQUEST_METHOD" , "execve");
	
	argv[0] = "/bin/sh";
	argv[1] = cgi_info->file->ptr;
	argv[2] = 0;
	//argv[0] = (char *)"/www/aa.cgi";

	int pid = fork();
	
	switch(pid) {
		case -1: 
			break;
		case 0: 
			dup2(infd[1], STDOUT_FILENO);
			
			close(infd[0]);
			close(outfd[1]);
			if(execve("/bin/sh", argv, environ) == 0) {
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
			
			cgi_info->pipe.in = infd[0];
			cgi_info->pipe.out = outfd[1];
			cgi_info->fd =  open(cgi_info->outfile->ptr, O_RDWR|O_CREAT|O_TRUNC|O_NONBLOCK, 0777);
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
	epoll_cgi_t * cgiInfo = (epoll_cgi_t *)palloc(cgi_info_manager->p, sizeof(epoll_cgi_t ));
	int nameSize = 30;
	char *name =(char *)palloc(cgi_info_manager->p,nameSize); //存储编译输出内容的文件
	snprintf(name, nameSize, "/tmp/devsync%d.out", ts);
	
	cgiInfo->file = string_init_from_str(cgi_info_manager->p, execute_file->ptr, execute_file->len);
	cgiInfo->last_add_ts = ts;
	cgiInfo->last_run_ts = 0;
	cgiInfo->status = CGI_STATUS_END;
	cgiInfo->outfile = string_init_from_ptr(cgi_info_manager->p, name, nameSize);
	hash_add_ptr(cgi_info_manager->p, cgi_info_manager->h, cgiInfo->file->ptr, cgiInfo->file->len, cgiInfo);
	return 0;
}

int get_cgi_operator_handle(epoll_cgi_t *cgi_info) {
	int size = 1024;
	int count ;
	char content[1024];
	

	 while((count = read(cgi_info->pipe.in, content, size)) > 0) {
		write(cgi_info->fd, content, count);
    }

}


