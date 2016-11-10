#ifndef _LINUX_EPOLL_H_
#define _LINUX_EPOLL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>



int epoll_init(long max);


int  epoll_edit_fd(int epfd, struct epoll_event *ev, int wr);

int  epoll_add_fd(int epfd, int fd, int wr, void *extra);


int epoll_del_fd(int epfd, struct epoll_event *ev);


#endif