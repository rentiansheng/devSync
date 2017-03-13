#include "cgi_handle.h"


int cgi_del(http_connect_t *con) {
	int pid = fork();
	if(pid  == 0) {
		buffer *file = buffer_init(con->p);
		buffer_append_str(file, con->in->uri->ptr, con->in->uri->len, con->p);
		buffer_append_char(file, 0, con->p);
		return execlp("rm", "rm", "-r", file->ptr, NULL);
	}
	return 0;
}


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
	int d = 0;
	int infd[2], outfd[2];//infd server read, outfd server write;
	
	cgi_ev = (cgi_ev_t *)pcalloc(p, sizeof(cgi_ev_t));

	cgi_info->p = p;
	pipe(infd);
	pipe(outfd);



	int pid = fork();

	switch(pid) {
		case -1: 
			break;
		case 0: 
			dup2(infd[1], STDOUT_FILENO);
			dup2(outfd[0], STDIN_FILENO);
			dup2(infd[1], STDERR_FILENO);
	
			close(infd[0]);
			close(outfd[1]);
			char title[100];
			char ts[30];
			
			getTime(ts, 30);
			snprintf(title, 100, "\n      %s    compilation result\n\n", ts);

			write(STDOUT_FILENO, title, strlen(title));
			
			char *argv[3] = {0} ;
			argv[0] = "/bin/bash";
			argv[1] = cgi_info->file->ptr;
			argv[2] = 0;

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
			make_fd_non_blocking(cgi_info->pipe.in);
			make_fd_non_blocking(cgi_info->pipe.out);


			epoll_add_fd(g->epfd, cgi_info->pipe.in, EPOLL_R, (void *)extra);	
			epoll_add_fd(g->epfd, cgi_info->pipe.out, EPOLL_R, (void *)extra);	

			break;
	}


}


int init_cgi_data_struct(string *execute_file, execute_cgi_info_manager_t *cgi_info_manager, unsigned int ts) {
	int i = 0;
	epoll_cgi_t * cgiInfo = (epoll_cgi_t *)palloc(cgi_info_manager->p, sizeof(epoll_cgi_t ));
	int nameSize = execute_file->len+30;
	char *fileName =(char *)palloc(cgi_info_manager->p,nameSize); //存储编译输出内容的文件

	execute_file->ptr[execute_file->len] = 0;
	snprintf(fileName, nameSize, "/tmp/devsync%s.out", execute_file->ptr);//ts);

	//跳过/tmp/devsync
	for(i = 12 ; i < strlen(fileName); i++) {
		if(fileName[i] == '/') {
			fileName[i] = '-';
		}
	}

	cgiInfo->file = string_init_from_str(cgi_info_manager->p, execute_file->ptr, execute_file->len);
	cgiInfo->last_add_ts = ts;
	cgiInfo->last_run_ts = 0;
	cgiInfo->status = CGI_STATUS_END;
	cgiInfo->outfile = string_init_from_ptr(cgi_info_manager->p, fileName, nameSize);
	hash_add_ptr(cgi_info_manager->p, cgi_info_manager->h, cgiInfo->file->ptr, cgiInfo->file->len, cgiInfo);
	return 0;
}

int get_cgi_operator_handle(epoll_cgi_t *cgi_info) {
	int size = 1024;
	int count ;
	char content[1025];
	

	 while((count = read(cgi_info->pipe.in, content, size)) > 0) {
		write(cgi_info->fd, content, count);
		content[count] = 0;
		printf("%s\n", content);
    }

}


