/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "ds_log.h"


char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
time_t ts ;

void getTime(char * ptr, int len) {
    struct tm * lts;
    time(&ts);
    lts = localtime(&ts); //取得当地时间
    snprintf(ptr, len, "%02d-%02d-%02d %02d:%02d:%02d", ((1900+lts->tm_year)%100), (1+lts->tm_mon), lts->tm_mday,lts->tm_hour, lts->tm_min, lts->tm_sec);

}


int ds_log(http_connect_t * con, char * str, int level) {
    pool_t *p = con->p;
    request * in = con->in;
    int ret = 0;
    buffer * log = (buffer *)buffer_create_size(p , (sizeof(char)*LOG_STR_LEN) );

    getTime(log->ptr, 18);
    log->used += strlen(log->ptr) ;
    //就是为了测试这两个函数的
    //buffer_append_char(log, ' ', p);
    buffer_append_str(log, " path[",  6, p);
    buffer_append_n_str(log, in->uri->ptr, in->uri->len, p);
    buffer_append_str(log, "] ip[", 5, p);
    buffer_append_n_str(log, in->clientIp->ptr, in->clientIp->len, p);
    buffer_append_char(log, ']', p);
    buffer_append_char(log, ' ', p);
    buffer_append_str(log, str, strlen(str), p);
    buffer_append_char(log, '\n', p);

    switch(level) {
        case 1:
            ret = write(logFileId, log->ptr, log->used);
            break;
        case 16:
            write(logFileErrorId,log->ptr, log->used);
            break;
    }

    return 0;
}



int ds_log_error(http_conf * conf, char * str) {

    return 0;
}


int ds_init(http_conf * conf) {
    logFileId = open("/tmp/devSync.log", O_RDWR|O_CREAT|O_APPEND, 0777);
    if(logFileId == -1) {
        printf("open devSync log failed\n");
    }
    logFileErrorId = open("/tmp/devSync_error.log", O_RDWR|O_CREAT|O_APPEND,0777);
    if(logFileErrorId == -1) {
        printf("open devSync error log failed\n");
    }

    return 0;
}
