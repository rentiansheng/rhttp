#include "str.h"


string * string_init(pool_t *p) {
	string  *str = NULL;
	
	str =(string  *) palloc(p, sizeof(string));
	
	str->ptr = NULL;
	str->len = 0;

	return str;
}

string * string_init_from_str(pool_t *p, char *b, size_t len) {
	string  *str = NULL;
	
	str =(string  *) palloc(p, sizeof(string));

	str->ptr = (void *)palloc(p, sizeof(char)*(len+1));
	str->len = len;
	memcpy(str->ptr, b, len);
    str->ptr[len] = 0;

	return str;
}

string * string_init_from_ptr(pool_t *p, char *ptr, size_t len) {
    string  *str = NULL;
	
	str =(string  *) palloc(p, sizeof(string));

    str->ptr = ptr;
    str->len = len;

    return str;
}


int string_compare(const string *s1, const string *s2){
	int len;

	if(s1->len == s2->len) return strncmp(s1->ptr, s2->ptr, s1->len);
    if(s1->len == 0) {
        return -1;
    }
    if(s2->len == 0) {
        return 1;
    }

	return s2->len - s1->len;//bug
	
	//return (s1->ptr[0] - s2->ptr[0]);
	
}

int string_compare_str(const string *s1, const char *s2) 
{
	int len = strlen(s2);
	if(len != s1->len) {
		return -1;
	}




	return strncmp(s1->ptr, s2, s1->len);
}


void string_copy_n_to_str(const string *b, char *s1, int n){
	if(b->ptr == NULL || b->len == 0) {*s1 = 0;return;}


	strncpy(s1, b->ptr, n);
	s1[n] = 0;
}

int  string_get_line(char  *start, char *end, string *dst) {

	char *p = start;
	dst->len = 0;

	if(start == NULL || start == NULL ) {
		return 0;
	}
	
	while(' ' == *p && p != NULL) {
		if(end >  p) {return 0;}
		p++;
	}
	if(p > end) {
		return 0;
	}


	while(*p != '\r' && *p != '\n' && p < end) {
		p++;
	}

	dst->ptr = start;
	dst->len = p - start;
	
	return 0;
}

void string_get_word_with_split(string * src, string *dst, char split) {

	char *p, *end;
	p = src->ptr;
	end = src->ptr + src->len;
	dst->len = 0;

	if(src == NULL || src->ptr == NULL || src->len == 0) {
		return ;
	}
	
	while(' ' == *p && p != NULL) {
		if(end >  p) {return ;}
		p++;
	}
	
	if(p > end) {
		return ;
	}

	while(*p != split && p < end) {
		p++;
	}

	dst->ptr = src->ptr;
	dst->len = p - src->ptr;

	return ;
}

void string_split_kv(string * src, string *key, string *value, char split) {

	char *p, *end;
	p = src->ptr;
	end = src->ptr + src->len;
	key->len = 0;

	if(src == NULL || src->ptr == NULL || src->len == 0) {
		return ;
	}
	
	while(' ' == *p && p != NULL) {
		if(end >  p) {return ;}
		p++;
	}
	
	if(p > end) {
		return ;
	}

	while(*p != split && p < end) {
		p++;
	}

	key->ptr = src->ptr;
	key->len = p - src->ptr;

	value->ptr = p + 1;
	value->len = src->len - key->len -1;
	if(value->len < 0) {
		value->len = 0;
	}

	return ;
}


void string_get_line_split(string *src, string *dst, char split)
{
	 char *p, *end;

	if(src == NULL || src->ptr == NULL) return;

	p = src->ptr;
	end =src->ptr + src->len;

	while(p < end){
		if(*p == split){
			dst->ptr = ++p;
			dst->len = end - p;
	 		return;
		}
	 	p++;
	}

	return ;

}

void string_skip_char_left(string *str, char c)
{
	if(str == NULL || str->len == 0 || str->ptr == NULL) {
		return ;
	}
	while(*str->ptr == c && str->len >0) {
		str->ptr += 1;
		str->len -= 1;
	}

}

void string_skip_char(string *str, char c) {
	if(str == NULL || str->len == 0 || str->ptr == NULL) {
		return ;
	}
	char *end = str->ptr + str->len;
	while(*str->ptr == c && str->len > 0) {
		str->ptr +=1;
		str->len -= 1;
	}
	while(*end == c && str->len > 0) {
		end--;
		str->len -= 1;
	}
}

int raw_str_ncmp(char * s1, char * s2, int len) 
{
	int len1 = strlen(s1);
	int len2 = strlen(s2);


	if(len1 == len2 && len == len1) return strncmp(s1, s2, len1);
    

	return len1-len2;//bug不知道谁大
	
}