#include "config.h"

char 
is_char(int c)
{
	if( ('Z' >= c && 'A' <= c) || ('z' >= c && 'a' <= c))
		return 1;
	return 0;
}


int set_auth(FILE *f, http_conf_t *conf, int *row) {
	char line[1024];
	int len = strlen(line);
	int err = read_line(f, line, row);
	if(err) { return 0;}

	string * auth = string_init_from_str(conf->p, line, len);

	conf->auth_usr = string_init(conf->p);
	conf->auth_pwd = string_init(conf->p);

	string_get_word_with_split(auth, conf->auth_usr, ':');
	if(conf->auth_usr->len == 0 || conf->auth_usr->ptr == NULL ) {
		return 26;
	}

	if(conf->auth_pwd->len == 0 || conf->auth_pwd->ptr == NULL) {
		return 27;
	}

	return 0;

}
/*
*读取配置文件的信息
*@paramter
*	path:配置文件存放的信息
*	globle_conf: 读取的配置文件内容存放的位置
*@result:
*/
int config_init(char *path, http_conf_t *conf)
{
	char line[1024];
	int row;
	int err;

	FILE * f = fopen(path, "r");

	if(NULL == f) return 1;//文件不存在
	
	while(!read_line(f, line, &row)){
		if(strlen(line) == 0 ) continue;
		if((err = parse_line(f, line, &row, conf))) return err;
	}

	return 0;
	
}





int parse_line(FILE *f, char *line,int  *row, http_conf_t *conf) 
{
	char *split;
	char *name;
	int err;
	
	name = split = strchr(line, '=');
	if(split == NULL) return 2;//配置文件错误
	
	while(!is_char(*name)) name++;


	if(strncmp(line, "port", 4) == 0){
		split ++;

		conf->port = atoi(split);
		if(conf->port <= 0 || conf->port > 65535)return 3;//port不正确
	}else if(strncmp(line, "web", 3) == 0) {
		if((err = set_web(f, conf, row))) return err;
		conf->web_count ++;
	}else if(strncmp(line, "mimetype", 8) == 0){
		if((err = set_mimetype(f, conf, row))) return err;
	} else if(strncmp(line, "auth", 4) == 0) {
		if(err = set_auth(f, conf, row) ) return err;

	}else {
		return 4;//不能识别的配置节点
	}

	return 0;
}




int set_mimetype(FILE * f, http_conf_t *conf, int *row)
{
	char line[1024];
	char *name, *split, *end;
	char *value;
	rkey_t *k;


	while(!read_line(f, line, row)){
		if(strlen(line) == 0) continue;
		name = line;
		split = strchr(name, ':');
		if(split == NULL) {
			if(strchr(name, ')') == NULL) return 12; else break;
		}
		end = split - 1;
		while(*end == ' ' &&  end >= name) end--;
		if(end < name) return 15;//配置节点的名字不能为空
		*(end + 1) = 0;


		value = split+1;
		while(*value == ' ') value++;
		end = split + strlen(split) - 1;
		while(*end == ' ' && end >= value)end --;
		if(end < value) return 15;//配置节点的名字不能为空
		*(end+1) = 0;
		
		k = (rkey_t *)pcalloc(conf->p, sizeof(rkey_t));
		k->name = string_init_from_str(conf->p, name, strlen(name));
		k->value =  string_init_from_str(conf->p, value, strlen(value));
		k->type = t_string;
		

		k->next = conf->mimetype;
		conf->mimetype = k;

	}
	return 0;
}


int set_web(FILE *f, http_conf_t *conf, int *row)
{
	char line[1024];
	char *name, *split, *end;
	char *value,  *item;
	struct web_conf *web;
	
	web = (struct web_conf_t *)pcalloc(conf->p, sizeof(web_conf_t));
	if(web == NULL) return MEMERROR;
	web->err404 = NULL;
	web->fd = 0;
	web->index_file = NULL;
	web->root = NULL;
	web->server = NULL;
	while(!read_line(f, line, row)){
		if(strlen(line) == 0) continue;
		name = line;
		split = strchr(name, ':');
		
		if(split == NULL)
		{
			 if(strchr(name, ')') == NULL)return 2; else break;
		}
		end = split - 1;
		while(*end == ' ' && end >= name) end--;
		if(end < name) return 15;//配置节点的名字不能为空
		*(end+1) = 0;
		
		
		value = split+1;
		while(*value == ' ') value++;
		end = split+strlen(split) - 1;
		while(*end-- == ' ' && end >=value)end --;
		if(end < value) return 15;//配置节点的名字不能为空
		*(end+1) = 0;
		
		if(raw_str_ncmp("root", value, 4) == 0) {
			web->root = string_init_from_str(conf->p, value, strlen(value));
		}
		else if(raw_str_ncmp( "indexfile", value, 10) == 0) {
			web->index_file = string_init_from_str(conf->p, value, strlen(value));
							
		}
		else if(raw_str_ncmp("404_page", value, 8) == 0) {
			web->err404 = string_init_from_str(conf->p, value, strlen(value));
		}
		else {
			return 14;//不能识别的配置节点
		}

	}
	
	web->next = conf->web;
	conf->web = web;

	return 0;	
}


int read_line(FILE *f, char *line, int *row)
{
	int zs = 0;
	int count;
	int c;
	
	row++;
	count = 0;
	while((c = fgetc(f)) !=EOF){
		if(zs == 1 && c != '\n') continue;
		if('#' == c){zs = 1;continue;}
		if(c == '\t' || c == '"') continue;
		if(c == '\n'){
			zs = 0;
			line[count] = 0;
			return 0;
		}
		if(c == ' ' && zs == 0)continue;
		line[count++] = c;
		zs = 2;
	}
	
	return 1;//read end

}

