/*
*读取配置文件的信息
*@paramter
*	path:配置文件存放的信息
*	globle_conf: 读取的配置文件内容存放的位置
*@result:
*/
int config_init(char *path, globle_conf *g);

void set_mimetype(FILE *f, globle_conf *g);

void set_web(FILE *f, globle_conf *g);

void set_globla(globle_conf *g, char *key, char *value);

void err_handler(FILE * f);
