/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "http_mod_authorized.h"

int http_mod_auth(http_connect_t *con)
{

    ds_log(con, "[ AUTH ]", LOG_LEVEL_ERROR);
    ds_log(con, con->auth->ptr, LOG_LEVEL_ERROR);
    ds_log(con, con->in->auth->ptr, LOG_LEVEL_ERROR);

    //      没有启用授权模式, 或者通过授权
    if (con->auth == NULL || (con->auth->len == con->in->auth->len && string_compare(con->auth, con->in->auth) == 0))
    {
        if (con->in->http_method == _PUT)
        {
            //open_write_file(con);
            con->next_handle = open_write_file;
        }
        else if (con->in->http_method == _GET)
        {
            con->next_handle = send_execute;
        }
        else if (con->in->http_method == _DEL)
        {
            con->next_handle = delete_resource;
        }
        else
        {
            con->next_handle = NULL; //最好输出不支持的信息
            return DONE;
        }
    }
    else
    {
        con->out->status_code = HTTP_UNAUTHORIZED;
        con->next_handle = send_unauthorized;
    }

    return NEXT;
}