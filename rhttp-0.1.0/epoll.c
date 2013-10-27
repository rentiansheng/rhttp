/*
*主要实现功能，epoll操作、socket非阻塞、socket链接的初始化
* 作者：Reage
* blog:http://blog.csdn.net/rentiansheng
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "base.h"
#include "epoll.h"
#include "request.h"
#include "response.h"
#include "buffer.h"

#define MAX_EPOLL 30000



/*
@description:开始服务端监听
@parameter
ip:web服务器的地址
port：web服务器的端口
@result：成功返回创建socket套接字标识，错误返回-1

*/
static int socket_listen( char *ip, unsigned short int port){
	int res_socket; //返回值
	int res, on;
	struct sockaddr_in address;
	struct in_addr in_ip;
	res = res_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(res_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET ;
	address.sin_port =htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
	res = bind( res_socket, (struct sockaddr *) &address, sizeof( address ) );
	if(res) { printf( "port is used , not to repeat bind\n" ); exit(101); };
	res = listen(res_socket,5000);
	if(res) { printf( "listen port is error ;\n" ); exit( 102 );  };
	return res_socket ;
}


/*设这文件句柄sfd为非阻塞
*
*/
static int make_socket_non_blocking (int sfd)
{
	int flags, s;
	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror ("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror ("fcntl");
		return -1;
	}

	return 0;
}

/*创建epoll管理最大文件句柄的个数
*/
static int epoll_init(long max)
{
	return epoll_create(max);
}

/*设这epoll管理每个文件句柄的可读触发
*parameter
* @fd：要管理socket文件句柄
*/
static struct epoll_event * epoll_event_init_r(int fd)
{
	struct epoll_event *events;
	events = (struct epoll_event *)malloc( sizeof(struct epoll_event) );
	events->data.fd = fd;
	events->events = EPOLLIN | EPOLLET;
	return events;
}

/*设这epoll管理每个文件句柄的可写触发
*parameter
* @fd：要管理socket文件句柄
*/
static struct epoll_event * epoll_event_init_w(int fd)
{
	struct epoll_event *events;
	events = (struct epoll_event *)malloc( sizeof(struct epoll_event) );
	events->data.fd = fd;
	events->events = EPOLLOUT | EPOLLET;
	return events;
}

/*将一个socket句柄添加到一个epoll中
*parameter
* @epfd: epoll句柄，
* @fd：要管理socket文件句柄
*/
static int epoll_handler(int epfd, int  fd)
{
	struct epoll_event *events = epoll_event_init_r(fd);
	struct epoll_event *ev = events;
	int i = 0;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, ev);
	return 0;
}




void handler_socket(struct globle_conf *g)
{
	int res_socket;
	int count ;
	int epfd ;
	int len ;
	long handler_max = (MAX_EPOLL/2);
	struct sockaddr_in client_addr;
	struct epoll_event *event=(struct epoll_event *)r_malloc(sizeof(struct epoll_event)*handler_max);
	

	res_socket = socket_listen( "127.0.0.1", g->port) ;
	make_socket_non_blocking(res_socket);

	epfd = epoll_init(MAX_EPOLL);
	epoll_handler(epfd, res_socket);

	while(1)
	{
		count = epoll_wait(epfd, event, handler_max, -1);
		while(count-- >0)
		{
			if(res_socket == event[count].data.fd)
			{
				int conn = accept(res_socket, (struct sockaddr *)&client_addr, &len);
				make_socket_non_blocking(conn);
				epoll_handler(epfd, conn);
			}
			else
			{
				struct request *req = (struct request *)r_malloc(sizeof(struct request));
				req->sockfd = event[count].data.fd;
				handler_request(req, g);
				response_handler(req, g);
				close(event[count].data.fd);
				buffer *header = req->header;
				//free(header->ptr);
				free(req);
			}
		}
	}
	
}

