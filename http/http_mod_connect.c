#include "http_mod_connect.h"



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

int socket_listen(char *ip, unsigned short int port) 
{
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
	if(res) {printf("port is used, not to repear bind %s\n", strerror(errno));}
	res = listen(res_socket, MAX_CONNECT);

	return res_socket;
}


int start_web_server(http_conf_t * conf)
{
	struct web_conf *web;
	int count ;
	int epfd;
	int fd = 0;
	epoll_extra_data_t *data;

	count = conf->web_count;
	web = conf->web;

	data = (epoll_extra_data_t *) malloc(sizeof(epoll_extra_data_t));

	data->type = SERVERFD;
	if(conf->port <=0) conf->port = 80;
	epfd = epoll_init(MAX_CONNECT);
	//while(count--){
	fd = socket_listen("127.0.0.1", conf->port);
	//	web->fd = fd;
	epoll_add_fd(epfd, fd,EPOLL_R, data);
	//	web = web->next;
	//}
	
	conf->fd = fd;
	conf->epfd = epfd;

	return epfd;
}