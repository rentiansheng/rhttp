#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "base.h"
#include <zlib.h>
#include <assert.h>


#define OS_CODE 0x03 /*Unix OS_CODE*/
#define DEFAULT_COMPRESSION Z_DEFAULT_COMPRESSION
#define DEFAULT_WINDOWSIZE -15
#define DEFAULT_MEMLEVEL 9
#define DEFAULT_BUFFERSIZE 8096
#define MAX 1024
#define METHOD 20 //获取数据的方式,使用字符串的长度
#define URI 255 //表示URI的最大长度
#define VERSION 15	//HTTP版本好的最大长度
#define TYPE 20     //表示文件的类型的长度




static const char gzip_header[10] = {'\037', '\213', Z_DEFLATED, 0, 0, 0, 0, 0, 0, OS_CODE};

void put_long (unsigned char *string, unsigned long x) {
	string[0] = (x & 0xff);
	string[1] = ((x >> 8) & 0xff) ;
	string[2] = ((x >> 16) & 0xff) ;
	string[3] = ((x >> 24) & 0xff);
}




int deflate_buffer (response *r) {



	z_stream stream;//zlib使用的。
	int ret, flush;
	buffer *out = r->out;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;
	
	
	//out->ptr = (char *) malloc(sizeof(char)*out_len);
	//out->size = out_len;
	//压缩初始化。
	int tmp_result = deflateInit2(&stream,
					DEFAULT_COMPRESSION,
					Z_DEFLATED,
					-MAX_WBITS,
					DEFAULT_MEMLEVEL,
					Z_DEFAULT_STRATEGY);
	if (Z_OK != tmp_result) {
		printf("deflateInit error: %d\r\n", tmp_result);
		return 0;
	}
	stream.avail_in = r->len;
	stream.next_in = r->start;

	buffer_prepare_copy(out, (stream.avail_in * 1.1) + 12);

	stream.avail_out = out->size;
	stream.next_out = out->ptr ;
	ret = deflate (&stream,Z_FINISH);
	assert (ret != Z_STREAM_ERROR);
	switch (ret) { 
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd (&stream);
			return ret;
	}
	out->used = stream.total_out;
	//write (sockd, out->ptr, out->used);
	if (Z_OK != deflateEnd(&stream)) {
		return -1;
	}
	return 1;
}



int gzip_buffer (response *r) {
	z_stream stream;//zlib使用的。
	int ret, flush;
	buffer *out = r->out;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;

	//压缩初始化。
	int tmp_result = deflateInit2(&stream,
					Z_DEFAULT_COMPRESSION,//压缩级别
					Z_DEFLATED,//压缩方式
					-MAX_WBITS,
					8,
					Z_DEFAULT_STRATEGY);
	if (Z_OK != tmp_result) {
		printf("deflateInit error: %d\r\n", tmp_result);
		return 0;
	}

	
	stream.avail_in = r->len; //要压缩数据的长度
	stream.total_in = 0;
	stream.next_in = r->start;	//要压缩数据的首地址

	buffer_prepare_copy(out, (stream.avail_in * 1.1) + 12 + 18);

	memcpy (out->ptr, gzip_header, 10);
	out->used = 10;
	stream.avail_out = out->size - 18 ;  //可存放的最大输出结果的长多。就是压缩后数据的最大长度
	stream.next_out = out->ptr + out->used; //存放压缩数据的开始位置，send前十个字节用来放头部
	ret = deflate (&stream,Z_FINISH); //压缩
	assert (ret != Z_STREAM_ERROR);
	switch (ret) { 
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd (&stream);
			return ret;
	}
	
	out->used += stream.total_out;
	unsigned crc = crc32(0L, r->start, r->len); 
	char * tail = out->ptr +out->used;
	put_long (tail, crc);
	put_long (tail + 4, r->len);
	out->used +=8;
	if (Z_OK != deflateEnd(&stream)) {
		return -1;
	}
	return 1;
}


int  read_filter(read_buffer *rb, char *msg, char split){
	if(0 >= rb->size )return 1;
	int len = strlen(msg);
	char *ptr = rb->ptr;
	char *end  ;
	char *lend = ptr + rb->size; 
	int count;
	while(NULL !=	ptr){
		end = (char *)strchr(ptr,split);
		count = end - ptr;
		if(NULL == end ) count = lend - ptr;
		if(count == len){
			if(!strncasecmp(ptr, msg, len))return 0;
		}
		ptr = end + 1;
		if(NULL == end){break;}
		if(' ' == *ptr)ptr++;	
	}
	return 1;
}


void hander_compress(response *r, read_buffer *compress){
	if(!read_filter(compress, "gzip", ',')){
		r->content_encoding = 2;
		gzip_buffer(r);
	}	
	else if( !read_filter(compress, "deflate", ',')){
		r->content_encoding = 1;
		deflate_buffer(r);
	}
	else{
		r->content_encoding = 0;
		buffer * out = (buffer *)r->out;
		out->ptr = r->start;
		out ->used = r->len ;
	}	
}

