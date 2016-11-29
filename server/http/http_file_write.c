/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
 #include "http_file_write.h"



int _mkdir(http_connect_t * con, char *root, pool_t *p)
{
    int len = strlen(root);
    char * dir = (char *) palloc(p, len+1);
    char *end = dir + len;
    struct stat buf;

    memcpy(dir, root, len+1);

    while(end > dir) {
        if(*end == '/') {
            *end= '\0';
            break;
        }

        end--;
    }

    if(stat(root, &buf) != 0 ) {
        _mkdir(con, dir, p);
    } else {
        return 0;
    }
    mkdir(root, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    return 0;
}


int  open_write_file(http_connect_t *con)
{
    request * in;
    response *out;
    buffer *uri, *tmp, *dir;
    char *ptr ;
    FILE * fp;
    pool_t *p = con->p;
    int count = 0;

    in = con->in;
    out = con->out;


    uri = buffer_init(p);
    uri->ptr = in->uri->ptr;
    uri->used = uri->size = in->uri->len;


    dir = buffer_create_size(con->p, in->uri->len);
    memcpy(dir->ptr, uri->ptr, uri->used);
    ptr = dir->ptr + uri->used;
    dir->used = uri->used;
    while(ptr >= dir->ptr) {
        if(*ptr == '/') {
            *ptr = '\0';
            break;
        }
        ptr--;
        dir->used--;

    }

    uri->ptr[uri->used] = '\0';
    if(dir->used) {
        int ret  = _mkdir(con, dir->ptr, p);
        if(ret == -1) {
            return DONE;
        }
    }

    ptr = (char *) palloc(p, sizeof(char)*2048);

    fp = fopen(uri->ptr, "w");

    if(fp) {
        make_fd_non_blocking(fileno(fp));
        con->write_file.fp = fp;
        con->write_file.len = 0;
        con->next_handle = write_file_content;
    } else {
        int logStrLen = strlen(dir) + 30;
        char * logStr = (char *) palloc(p, logStrLen);
        snprintf(logStr, logStrLen, "[access %s Permission denied]", dir );
        ds_log(con, logStr, LOG_LEVEL_ERROR);
        con->next_handle = send_put_forbidden_result;
    }


    return NEXT;
}

int  write_file_content(http_connect_t * con)
{
    request * in;
    response *out;
    
    char *ptr ;
    pool_t *p = con->p;
    int count = 0;

    ptr = palloc(p, sizeof(char)*CONTENT_SIZE);
    in = con->in;
    out = con->out;
    while((count = read(con->fd, ptr, CONTENT_SIZE)) > 0) {
        if(fwrite(ptr, count, 1, con->write_file.fp) != 1) {
            fclose(con->write_file.fp);
            con->next_handle =  send_put_result;
            con->out->status_code = HTTP_WRITE_FILE_FAIL;

            return NEXT;
            //return con->next_handle(con);
        } 
        con->write_file.len += count;
    }

    //尽量的避免文件内容大小，比约定的文件大，出现这种有可能是编码的问题
    //这个中方法只能减少，并不能从根本上避免问题
    if(in->content_length <= con->write_file.len && (count == 0 || count == -1)) {
         fclose(con->write_file.fp);
         con->next_handle =  send_put_result;
         con->out->status_code = HTTP_OK;

         return NEXT;
         //return con->next_handle(con);

    }

    return CONTINUE;
}






