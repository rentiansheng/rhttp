#ifndef __HTTP_MOD_CONNECT_H_
#define _HTTP_MOD_CONNECT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#define MAX_CONNECT 10000
#define EPOLL_W 1
#define EPOLL_R 2

int socket_listen(char *ip, unsigned short int port);


int epoll_init(long max);


int  epoll_edit_fd(int epfd, struct epoll_event *ev, int wr);

int  epoll_add_fd(int epfd, int fd, int wr, void *extra);


int epoll_del_fd(int epfd, struct epoll_event *ev);

#endif

