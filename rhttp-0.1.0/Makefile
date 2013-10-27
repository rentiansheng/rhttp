
all:reagehttpd
	
reagehttpd:main.c config.o memory.o epoll.o  request.o buffer.o response.o mod_compress.o key.o
	gcc -g -o $@ $^ -lz

epoll.o:epoll.c
	gcc -g -c -o $@ $^

config.o:config.c
	gcc -g -c -o $@ $^

memory.o:memory.c
	gcc -g -c -o $@ $^
request.o:request.c 
	gcc -g -c -o $@ $^
buffer.o:buffer.c
	gcc -g -c -o $@ $^
response.o:response.c 
	gcc -g -c -o $@ $^
mod_compress.o:mod_compress.c 
	gcc -g -c -o $@ $^ -lz
key.o:key.c
	gcc -g -c -o $@ $^

clean:
	rm -f reagehttpd
	rm -f *.o
	
