#include "http_execute_sh.h"

int  send_execute_sh_cmd(http_connect_t * con, http_conf * g )
{
    buffer *header = buffer_create_size(con->p, 1024);
    time_t t  = time(NULL) ;
    
    
    
    //buffer_append_str(header, "execute:", 8, con->p);
    buffer_append_n_str(header, con->in->execute_file->ptr, con->in->execute_file->len, con->p);
    buffer_append_char(header, '\n', con->p);
    //buffer_append_str(header, "time:", 5, con->p);
    buffer_append_long(header, t, con->p);
    buffer_append_char(header, '\n', con->p);
    buffer_append_char(header, '\n', con->p);
    write(g->child_pip.out, header->ptr, header->used);
    
    return 0;

}

/*int send_get_content_cmd(http_connect_t *con, http_conf *g) {
    buffer_append_str(header, "service:", 8, con->p);
    buffer_append_n_str(header, "list ", 0, con->p);
    buffer_append_char(header, '\n', con->p);
    buffer_append_str(header, "time:", 5, con->p);
    buffer_append_long(header, t, con->p);
    buffer_append_char(header, '\n', con->p);
    buffer_append_char(header, '\n', con->p);
    write(g->child_pip.out, header->ptr, header->used);
}*/