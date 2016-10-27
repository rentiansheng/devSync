#include "cgi.h"


execute_cgi_info_manager_t * initCgiManager() {
	pool_t * cgi_pool = (pool_t *)pool_create();
	hash_t *h = hash_init(cgi_pool);
	execute_cgi_info_manager_t *execute_cgi_info 
		= (execute_cgi_info_manager_t *)pcalloc(cgi_pool, sizeof(execute_cgi_info_manager_t));
	execute_cgi_info->p = cgi_pool;
	execute_cgi_info->h = h;

	return execute_cgi_info;
}

//0header读取完成。1数据未完成
int read_cgi_header(buffer *header, int fd) {
    char c;

    while(read(fd, &c, 1) == 1) {

        if(header->used < header->size) {
            header->ptr[header->used++] = c;
            if(c == '\n' && header->used >= 2) {
                char * ptr =  header->ptr + header->used - 2;
                if(strncasecmp(ptr, "\n\n", 2) == 0) {
                    return 0;
                }
            }
        }
		
	}


	return 1;
}

int parse_cig_header(execute_cgi_info_manager_t  * cgi_info_manager, buffer * header) {
    if(header->ptr == NULL || header->used == 0) {
        return 0;
    }

    char *start = header->ptr;
    char *end = start+header->used;
    unsigned int ts ;
    string file ;
    while(start < end && *start != '\n') { start ++;}
    
    file.ptr = header->ptr;
    file.len = start - header->ptr;
    if(*start == '\n') {
        //string * file = string_init_from_str(cgi_info_manager->p, header->ptr, start- header->ptr);
        hash_item_t * hitem = get_hash_item(cgi_info_manager->h, file.ptr, file.len);
        ts = atoi(start+1);
        if( hitem) {
            epoll_cgi_t *cgi_info = (epoll_cgi_t *) hitem->value.ptr;            
            if(ts > cgi_info->last_add_ts) {
                cgi_info->last_add_ts = ts;
            }
        } else {
           
            init_cgi_data_struct(&file, cgi_info_manager, ts);
        }

        
    }

    return 0;


}



int start_cgi_server(http_conf *g, execute_cgi_info_manager_t * cgi_info_manager) {
    epoll_extra_data_t *data ;
    int epfd;

    data = (epoll_extra_data_t *) palloc(cgi_info_manager->p, sizeof(epoll_extra_data_t));
    data->type = CGISERVERFD;
    
    make_fd_non_blocking(g->child_pip.in);
    g->fd = g->child_pip.in;
    g->epfd = epoll_init(MAX_CONNECT);

    epoll_add_fd(g->epfd, g->fd, EPOLL_R, data);

    return epfd;
}

static void signalhld_handle(int sig) {
    hash_item_t * hitem;
    epoll_cgi_t * executeCgi;
    int i;

    int pid = wait(NULL);
    for(i = 0; i < cgi_info_manager_gloable->h->size; i++) {
        hitem = cgi_info_manager_gloable->h->buckets[i].items;
        while(hitem) {
            if(hitem->type == HASH_ITEM_VALUE_TYPE_PTR) {
                executeCgi = (epoll_cgi_t *)hitem->value.ptr;
                if(executeCgi->pid == pid ) {
                    executeCgi->status = CGI_STATUS_END;
                    executeCgi->pid = 0;
                }
            }
            hitem = hitem->next;
        }
    }
}

int start_cgi(http_conf *g) {

    execute_cgi_info_manager_t * cgi_info_manager ;
    struct epoll_event ev[MAX_EVENT];
	struct epoll_event *evfd ;
    int count;
    buffer *header;
	epoll_extra_data_t *epoll_data;
    int i = 0;
    hash_item_t * hitem;
    epoll_cgi_t * executeCgi;
    int evIndex;

    cgi_info_manager = cgi_info_manager_gloable = initCgiManager();
    header = buffer_create_size(cgi_info_manager->p, 1024);
    start_cgi_server(g, cgi_info_manager);
    printf("--------------- start execute sh server\n");
    signal(SIGCHLD, signalhld_handle);
    while(1) {
        count = epoll_wait(g->epfd, ev, MAX_EVENT, -1);
        //收集所有需要需要执行cgi文件的
        while(read_cgi_header(header, g->fd) == 0) {
            parse_cig_header(cgi_info_manager, header);
            header->used = 0;
        }

        //处理正在执行cgi文件
		if(count < 0) { count = 0;}
        evfd = ev;

		for(evIndex = 0; evIndex < count; evIndex++){
            
            if(evfd->events & EPOLLIN) {
                epoll_data  = ev->data.ptr;
                switch(epoll_data->type) {
                    case CGIFD: 
                    get_cgi_operator_handle((epoll_cgi_t *)epoll_data->ptr);
                    break;
                }
            }
            
            evfd++;
        }

        //开始执行cgi任务, 其实可以用回调函数或者hash_filter返回可用的数据。然而我不回写如何将函数做为参数

        for(i = 0; i < cgi_info_manager->h->size; i++) {
            hitem = cgi_info_manager->h->buckets[i].items;
            while(hitem) {
                if(hitem->type == HASH_ITEM_VALUE_TYPE_PTR) {
                    executeCgi = (epoll_cgi_t *)hitem->value.ptr;
                    if(executeCgi->last_run_ts < executeCgi->last_add_ts) {//执行时间小于
                        cgi_handle(executeCgi, g);
                    }
                }
                hitem = hitem->next;
            }
        }



    }
   
}