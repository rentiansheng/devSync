#include "ds_daemon.h"


int ds_daemon(http_conf * conf)
{
    int ds_pid = 0;
    int fd;
    int uid = 0;

    uid = getuid();
    switch(fork()) {
        case -1:
            printf("fork failed!" DS_LINEEND);
        break;
        case 0:
        break;
        default:
            exit(0);
        break;
    }

    ds_pid = getpid();
    if(setsid() == -1) {  //setsid创建一个新会话
        printf("setsid() failed!" DS_LINEEND);
        exit(0);
    }

    umask(0);
    setuid(uid);
    //打开文件/dev/null,使得其拥有守护进程的0，1，2。这样防止守护进程在终端设备上显示输出
    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        printf("open(\"/dev/null\") failed" DS_LINEEND);
        exit(0);
    }
    //为了保存正可以输出错误信息，标准输出放到最后的

    if (dup2(fd, STDIN_FILENO) == -1) {
        printf("dup2(STDIN) failed" DS_LINEEND);
        exit(0);
    }

    fd = open("/tmp/devSync_error.log", O_RDWR|O_CREAT, 0777);
    if (dup2(fd, STDERR_FILENO) == -1) {
        printf("dup2(STDERR) failed" DS_LINEEND);
        exit(0);
    }

    fd = open("/tmp/devSync.log", O_RDWR|O_CREAT,0777);
    if (dup2(fd, STDOUT_FILENO) == -1) {
        printf("dup2(STDOUT) failed" DS_LINEEND);
        exit(0);
    }

    return OK;
}
