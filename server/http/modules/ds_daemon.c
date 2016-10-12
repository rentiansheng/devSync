/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "ds_daemon.h"

int forkPid  = 0;

static int ds_init_daemon(http_conf * conf) 
{
      int fd;
    
     if( socket_listen_test("127.0.0.1", conf->port)  ){
        exit(0);
    }
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

    if (dup2(fd, STDERR_FILENO) == -1) {
        printf("dup2(STDERR) failed" DS_LINEEND);
        exit(0);
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        printf("dup2(STDOUT) failed" DS_LINEEND);
        exit(0);
    }

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
}


static ds_init_children(http_conf * conf) 
{
    while(1) {
        forkPid = fork();

        if(forkPid == 0) {
            break;
        } else {
            waitpid(-1, 0, 0);
        }
    }
}


int ds_daemon(http_conf * conf, int t)
{


    int ds_pid = 0;
  
    int uid = getuid();
    int gid = getgid();
    int status = 0;


    if(conf->user && strlen(conf->user)) {
        struct passwd * pw  = getpwnam(conf->user);
        if(!pw) {
            printf(" user[%s] no found\n", conf->user);
            exit(0);
        }
        uid = pw->pw_uid;
        gid = pw->pw_gid;
        printf("\n user:%s\n", conf->user);
    }


    if(t == 0 ) {
        ds_init_daemon(conf);
    }
   

   if( t == 0 || t == 1) {
       ds_init_children(conf);

       ds_pid = getpid();

        if(setsid() == -1) {  //setsid创建一个新会话
            printf("setsid() failed!" DS_LINEEND);
            exit(0);
        }
        
        umask(0);
        setuid(uid);
        setgid(gid);
        ds_init(conf);
   }
 

    


    return OK;
}
