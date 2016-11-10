/*
 * Copyright (C) Reage
 * blog:http://www.ireage.com
 */
#include "http_deamon.h"


int socket_listen_test(char *ip, unsigned short int port)  {
	int res_socket;
	int res, on;
	struct sockaddr_in address;
	//struct in_addr in_ip;
	
	on = 1;
	res = res_socket = socket(AF_INET, SOCK_STREAM, 0);
	res = setsockopt(res_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	res = bind(res_socket, (struct sockaddr *)&address, sizeof(address));
	if(res) {printf("port is used, not to repear bind %s\n", strerror(errno)); return -1;}

	return 0;
}

static 
int http_init_daemon(struct http_conf * conf) 
{
    int fd;
    
    if( socket_listen_test("127.0.0.1", conf->port)  ){
        exit(0);
    }
    //打开文件/dev/null,使得其拥有守护进程的0，1，2。这样防止守护进程在终端设备上显示输出
    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        printf("open(\"/dev/null\") failed\n");
        exit(0);
    }
    //为了保存正可以输出错误信息，标准输出放到最后的

    if (dup2(fd, STDIN_FILENO) == -1) {
        printf("dup2(STDIN) failed\n");
        exit(0);
    }

    if (dup2(fd, STDERR_FILENO) == -1) {
        printf("dup2(STDERR) failed\n");
        exit(0);
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        printf("dup2(STDOUT) failed\n");
        exit(0);
    }

    switch(fork()) {
        case -1:
            printf("fork failed!\n");
        break;
        case 0:
        break;
        default:
            exit(0);
        break;
    }
}


static http_init_children(struct http_conf * conf) 
{
    int pid = 0;//0标识运行accept和cgi， 1 标识只允许faccept, 2fork cgi进程

    int status;
    while(1) {
        
        pid = fork();
        if(pid == 0) {
            break;
        } 

        int exitPid = wait(&status);

        printf("exit child %d status %d\n, to restart\n", exitPid, status);

    }
}


int http_daemon(struct http_conf * conf)
{  
    int uid = getuid();
    int gid = getgid();
    int status = 0;

    http_init_daemon(conf);
    http_init_children(conf);


    return 0;
}
