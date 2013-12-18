all:reagehttp

MYHEAD = -I core/ -I http/
CFLAGS = -g -W -Wall $(MYHEAD) -o

OBJ=reagehttp.o config.o config.o buffer.o pool.o http_mod_connect.o \
	http_request.o

reagehttp:$(OBJ)
	gcc  $(CFLAGS) $@ $^

reagehttp.o:http/reagehttp.c
	gcc -c $(CFLAGS) $@ $^

config.o:http/config.c
	gcc  -c $(CFLAGS) $@ $^

buffer.o:core/buffer.c
	gcc -c $(CFLAGS)  $@ $^

pool.o:core/pool.c
	gcc -c $(CFLAGS)  $@ $^

http_mod_connect.o:http/http_mod_connect.c
	gcc -c $(CFLAGS) $@ $^

http_request.o:http/http_request.c
	gcc -c $(CFLAGS) $@ $^


clean:
	rm -rf *.o















