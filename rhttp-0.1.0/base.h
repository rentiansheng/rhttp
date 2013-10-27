typedef struct web_conf{
	char *root;	//web site path
	char *index_file;
	char *err404;
	char *server;
	struct web_conf * next;
}web_conf;

typedef struct globle_conf{
	//char *mod_path;//模块的so文件放的位置
	int web_count;
	int port;
	struct KEY *mimetype;
	struct web_conf *web_config;
}globle_conf;

typedef enum { 
	t_char,
	t_uchar,
	t_int,
	t_uint,
	t_long,
}key_value_type;

typedef struct KEY{
	char *name;
	void *value;
	key_value_type data_type;
	struct KEY *next;
}KEY;

typedef enum{
	get,
	post,
	none
}http_method_t;

typedef struct response{
	int status_code;
	int content_encoding;//0 none 1 deflate 2 gzip	
	void * start;
	size_t len;
	struct read_buffer * compress;
	struct read_buffer * content_type;
	struct buffer * out;
	
	
}response;

typedef struct request{
	struct buffer *uri;
	struct read_buffer *host;
	http_method_t http_method;
	struct read_buffer * http_version;
	struct read_buffer * compress;
	struct buffer *header;
	int sockfd;
	struct web_conf * web;
	struct read_buffer * pages;
}request;
