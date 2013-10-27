#include <stdlib.h>

typedef struct {
	char *ptr;

	size_t used;
	size_t size;
} buffer;

typedef struct {
	buffer **ptr;

	size_t used;
	size_t size;
} buffer_array;

typedef struct {
	char *ptr;
	size_t size;
} read_buffer;




read_buffer * read_buffer_init();

read_buffer * read_buffer_init_str(char *b, size_t len);
/* turn on in buffer.h*/
void  read_buffer_get_string(char *s, read_buffer *b);

/* turn on in buffer.h*/
void  read_buffer_get_line(char *s, read_buffer *b);

void free_read_buffer(read_buffer * b);





buffer * buffer_init();






buffer* buffer_init_buffer(buffer *src);
buffer * buffer_init_string(char * str);

void  buffer_clear(buffer *b);

void buffer_free(buffer *b);

int buffer_prepare_int(buffer * b, int size);

int buffer_prepare_append(buffer *b, size_t size);

int buffer_copy_string(buffer *b, const char *s);

int buffer_copy_string_len(buffer *b, const char *s, size_t s_len);

int buffer_append_string_buffer(buffer *b, const buffer *src);

int buffer_append_string(buffer *b, const char *s);

int buffer_append_memory(buffer *b, const char *s, size_t s_len);

int buffer_copy_memory(buffer *b, const char *s, size_t s_len);

int buffer_to_lower(buffer *b);

int buffer_to_upper(buffer *b);

int buffer_copy_string_buffer(buffer *b, const buffer *src) ;

int buffer_is_equal(buffer *a, buffer *b);

int buffer_caseless_compare(buffer *a, buffer *b) ;

int buffer_is_equal_len(buffer *a, buffer *b, size_t len) ;

int buffer_caseless_compare_len(buffer *a, buffer *b) ;



buffer_array* buffer_array_init(void) ;

/**
 * free the buffer_array
 *
 */

void buffer_array_free(buffer_array *b) ;


buffer * buffer_array_append(buffer_array *b);
	
int buffer_path_simplify(buffer *dest, const buffer *src);

int buffer_prepare_copy(buffer *b, size_t size) ;
	
