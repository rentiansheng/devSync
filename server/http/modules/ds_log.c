#include "ds_log.h"


char * getTime(char * format) {

}


int ds_log(http_conf * conf, char * str, int level) {
    switch(level) {
        case 1:
            write(logFileId, str, strlen(str));
            break;
        case 16:
            write(logFileErrorId, str, strlen(str));
            break;
    }

    return 0;
}



int ds_log_error(http_conf * conf, char * str) {

    return 0;
}


int ds_init(http_conf * conf) {
    logFileId = open("/tmp/devSync_error.log", O_RDWR|O_CREAT|O_APPEND, 0777);
    if(logFileId == -1) {
        printf("open devSync log failed\n");
    }
    logFileErrorId = open("/tmp/devSync.log", O_RDWR|O_CREAT|O_APPEND,0777);
    if(logFileErrorId == -1) {
        printf("open devSync error log failed\n");
    }
}
