#include "http_send_page.h"

int
send_unauthorized(int fd)
{
	char msg[2048];
	
	sprintf(msg, "HTTP/1.1 401 Unauthorized Reqired\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);
	sprintf(msg, "%sWWW-Authenticate: Basic realm=\"%s\"\r\n\r\n<html><body>sss</body><html>", msg, _Auth_desc);
	
	
	return write(fd, msg, strlen(msg));
}

int 
send_not_find(int fd)
{
	char msg[1024];
	sprintf(msg, "HTTP/1.1 404 Not Found\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);
	sprintf(msg, "%sWWW-Authentlcate: Basic realm=\"%s\"\r\n\r\n", msg, _Auth_desc);
	    
	sprintf(msg, "%s<html><head></head><body><h1>404 Not Found</h1></body>\r\n\r\n", msg);
	     
	return write(fd, msg, strlen(msg));
}

int 
send_bad_gateway(int fd)
{
	char msg[1024];
	sprintf(msg, "HTTP/1.1 502 Bad Gateway\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);
	sprintf(msg, "%sWWW-Authentlcate: Basic realm=\"%s\"\r\n\r\n", msg, _Auth_desc);
	    
	sprintf(msg, "%s<html><head></head><body><h1>502 Bad Gateway</h1></body>\r\n\r\n", msg);
	     
	return write(fd, msg, strlen(msg));
}


int 
send_bad_request(int fd)
{
	char msg[1024];
	sprintf(msg, "HTTP/1.1 400 Bad Request\r\nConnection: Close\r\nContent-Type: text/html\r\n");
	if(_Server != NULL)	sprintf(msg, "%sServer: %s\r\n", msg, _Server);
	    
	sprintf(msg, "%s<html><head></head><body><h1>400 Bad Request</h1></body>\r\n\r\n", msg);
	     
	return write (fd, msg, strlen(msg));
}
