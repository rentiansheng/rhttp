/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "http_daemon.h"

static int http_init_daemon(http_conf * conf) 
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


static http_init_children(http_conf * conf) 
{
    int pid = 0;//0标识运行accept和cgi， 1 标识只允许faccept, 2fork cgi进程

    int status;
    while(1) {
        
        pid = fork();
        if(pid == 0) {
            conf->child_pip.in = pipHttp[0];
            conf->child_pip.out = pipCgi[1];
            close(pipHttp[1]);
            close(pipCgi[0]);
            conf->work_mode = FORK_PROCESS_WORK_HTTP_MODE;
            break;
        } 

        int exitPid = wait(&status);

        printf("exit child %d status %d\n, to restart\n", exitPid, status);

    }
}


int http_daemon(http_conf * conf)
{  
    int uid = getuid();
    int gid = getgid();
    int status = 0;

    http_init_daemon(conf);
    http_init_children(conf);


    return OK;
}
