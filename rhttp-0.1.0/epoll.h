/*
*主要实现功能，epoll操作、socket非阻塞、socket链接的初始化
* 作者：Reage
* blog:http://blog.csdn.net/rentiansheng
*/


/*
@description:开始服务端监听
@parameter
ip:web服务器的地址
port：web服务器的端口
@result：成功返回创建socket套接字标识，错误返回-1

*/
static int socket_listen( char *ip, unsigned short int port);


/*设这文件句柄sfd为非阻塞
*
*/
static int make_socket_non_blocking (int sfd);

/*创建epoll管理最大文件句柄的个数
*/
static int epoll_init();

/*设这epoll管理每个文件句柄的参数和方法
*parameter
* @fd：要管理socket文件句柄
*/
static struct epoll_event * epoll_event_init(int fd);


/*将一个socket句柄添加到一个epoll中
*parameter
* @epfd: epoll句柄，
* @fd：要管理socket文件句柄
*/
static int epoll_handler(int epfd, int  fd);


void handler_socket(struct globle_conf *g);
