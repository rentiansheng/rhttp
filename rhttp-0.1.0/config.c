#include <stdio.h>
#include "base.h"
#include <string.h>
#include "memory.h"
#include <stdlib.h>
#include "config.h"



char is_char(int c)
{
	if( ('Z' >= c && 'A' <= c) || ('z' >= c && 'a' <= c))
		return 0;
	return 1;
}

/*
*读取配置文件的信息
*@paramter
*	path:配置文件存放的信息
*	globle_conf: 读取的配置文件内容存放的位置
*@result:
*/
int config_init(char *path, globle_conf *g)
{
	char zs = 0;//是否为注释
	char p_node = 0; //是否位父节点
	char *key = NULL, *value = NULL;
	char *parent;
	int count = 0;
	FILE * f = fopen(path, "r");
	parent = (char *) malloc(sizeof(char) * 1024);
	int c;
	if(NULL == f) return -1;
	while((c = fgetc(f)) !=EOF)
	{
		if(zs == 1 &&  '\n' != c  ) continue;
		if(zs == 1 && '\n' == c  ) {zs = 0;continue;}
		if('#' == c){zs = 1;continue;}
		if(' ' == c)
		{
			continue;
		}
		else if('=' ==  c)
		{
			if(0 == strncmp(parent, "mimetype", 8))
			{
				set_mimetype(f,g);
			}
			else if(0 == strncmp(parent, "web", 3))
			{
				set_web(f,g);
				(g->web_count) ++;
			}
			//else if(0 == strncmp(parent, "mod_path", 8))
			//{
			//	parent[count] = 0;
			//	if(key == NULL )
			//	{
			//		key = (char *) r_malloc( count *sizeof(char) +1);
			//	}
			//	strcpy(key, parent);
			//}
			else if(0 == strncmp(parent, "port", 4))
			{
				parent[count] = 0;
				if(key == NULL )
				{
					key = (char *) r_malloc( count *sizeof(char) +1);
				}
				strcpy(key, parent);
			}
			else
			{
				printf("%s is not used\n", parent);
				err_handler(f);
			}
			count = 0;
		}
		else if( '\n' == c )
		{
			parent[count] = 0;
			if(NULL == key)
			{
				continue;
				printf("\nerror value \"%s\"\n", key);
			}
			else if(0 == *key || 0 == parent[0])
			{
				printf("\nerror \"%s\"\n", key);
				continue;
			}
			else if(parent[0] && *key)
			{
				set_globla(g, key, parent);
			}
			if(key)free(key);key  = NULL;
			count = 0;
		}
		else
		{
			parent[count++] = c;
		}
	}
	return 0;
	
}

void err_handler(FILE * f)
{
	int c;
	while((c = fgetc(f)) !=	 EOF)
	{
		if('\n' == c)break;
	}
}


struct web_conf * web_init()
{
	return (struct web_conf *) r_malloc(sizeof(struct web_conf));
}


/*
* 判断是否以（开始
*/
void globla_node_is_start(FILE *f)
{
	int c;
	while((c = fgetc(f)) !=EOF)
	{
		if( ' ' != c)
		{
			if( '(' !=  c)
			{
				printf("configure file is error.\n");
				exit(0);
			}
			else
			{
				break;
			}
		}
	}

}


void set_mimetype(FILE *f,globle_conf *g)
{
	int count = 0;
	int c;
	char tmp[100];
	char *c_key = NULL;
	char bl = 0;
	char row_null = 1;
	struct KEY *k = NULL;

	tmp[0] = 0;
	globla_node_is_start(f);
	
	while((c = fgetc(f)) !=EOF)
	{
		if(' ' != c && '\t' != c)
		{
			if('"'== c  )
			{
				tmp[count] = 0;
				count = 0;
				continue;
			}
			else if(':' == c)
			{
				c_key=(char *)r_malloc(sizeof(char)*strlen(tmp));
				strcpy(c_key,tmp);
			}
			else if('\n' == c)
			{
				if(1 != row_null )
				{
					if( NULL == c_key || 0 == strlen(c_key))
					{
						printf("%s is error\n", tmp);
						free(c_key);
						c_key = NULL;
						continue;
					}
					else if(0 == strlen(tmp))
					{
						printf("%s is error\n", c_key);
						free(c_key);
						c_key = NULL;
						continue;
					}


					k = (struct KEY *)r_malloc(sizeof(struct KEY ));
					k->name = (char *) r_malloc(sizeof(char)*strlen(c_key));
					k->value = (char *) r_malloc(sizeof(char)*strlen(tmp));

					strcpy(k->name,c_key);
					strcpy(k->value,tmp);
				
					k->next = g->mimetype;
					g->mimetype = k;
									
					free(c_key);
					c_key = NULL;
				}
				row_null = 1;

			}
			else if(')' == c)
			{
				if(1 != row_null)
				{
					printf("config file is error!\n");
					exit(0);
				}
				break;
			}
			else
			{
				tmp[count++] = c;
				row_null = 0;
			}
		
		}
	}


	
}

















void set_web(FILE *f,globle_conf *g)
{
	int count = 0;
	int c;
	char tmp[100];
	char *key = NULL;
	char bl = 0;
	struct web_conf * web = web_init();
	
	globla_node_is_start(f);

	
	while((c = fgetc(f)) !=EOF)
	{
		if(' ' != c && '\t' != c)
		{
			if(':' == c)
			{
				if(0 == count)
				{
					printf("error in configure\n");
					exit(0);
				}
				tmp[count] =  0;
				if(key) free(key);
				key = (char *) r_malloc(sizeof(char)*count + 1);
				strncpy(key,tmp,count+1);
				count = 0;
			}
			else if('\n' == c)
			{
				if(0 == count )
				{
					if( NULL != key)
					{
						printf("error node \"%s\" in configure\n",key);
						exit(0);
					}
					else
					{
						continue;
					}
				}
				tmp[count++] = 0;
				if(0 == strcmp(key, "root"))
				{
					web->root = (char *) r_malloc(sizeof(char) * count);
					strncpy(web->root,tmp, count);
				}
				else if(0 == strcmp(key, "indexfiles"))
				{
					web->index_file = (char *) r_malloc(sizeof(char) * count);
					strncpy(web->index_file,tmp, count);
				}
				else if(0 == strcmp(key, "host"))
				{
					web->server = (char *) r_malloc(sizeof(char) * count);
					strncpy(web->server,tmp, count);
				}
				else if(0 == strcmp(key, "404_page"))
				{
					web->err404 = (char *) r_malloc(sizeof(char) * count);
					strncpy(web->err404,tmp, count);
				}
				/*else if(0 == strcmp(key, "port"))
				{
					web->port = atoi(tmp);
					if(web->port <= 0)
					{
						printf("%d is not used port\n", web->port);
						exit(0);
					}
				}*/
				else
				{
					printf("error %s\n",key);
				}
				free(key);
				key = NULL;
				bl = 0;
				count=0;
			}
			else 
			{
				if(')' == c)
				{
					if(NULL != 	key )
					{
						printf("error in configure\n");
						exit(0);
					}
					else
					{
						break;
					}
				}
				tmp[count++] = c;
			}
		
		}
	}
	if(key)free(key);
	web->next = g->web_config;
	g->web_config = web;
	
}



void set_globla(globle_conf *g, char *key, char *value)
{
	if(0 == strcmp(key, "port"))
	{
		g->port = atoi(value);
	}
	else
	{
		printf("\nerror node \"%s\"\n", key);
	}
}
