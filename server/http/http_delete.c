
/*
 * Copyright (C) Reage
 * BLOG: http://www.ireage.com
 */

#include "http_delete.h"

int delete_resource(http_connect_t *con)
{
    response *out;
    buffer *tmp, *dir;
    char *ptr;
    FILE *fp;
    pool_t *p = con->p;

    string *uri = string_init_from_str(p, con->in->uri->ptr, con->in->uri->len);

    if (access(uri->ptr, 0))
    {
        con->next_handle = send_put_result;
        con->out->status_code = HTTP_OK;
    }
    else
    {
        int errNO = remove(uri->ptr);
        if (errNO != 0)
        {
            int logStrLen = strlen(uri) + 30;
            char *logStr = (char *)palloc(p, logStrLen);
            snprintf(logStr, logStrLen, "[remove %s error. errNo:%d]", uri->ptr, errNO);
            ds_log(con, logStr, LOG_LEVEL_ERROR);
            con->next_handle = send_put_forbidden_result;
        }
        else
        {
            con->next_handle = send_put_result;
            con->out->status_code = HTTP_OK;
        }
    }

    return NEXT;
}
