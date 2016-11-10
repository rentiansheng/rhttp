all:obj/reagehttp



MYHEAD = -I core/ -I http/ -I linux/
CFLAGS = -g -W -Wall $(MYHEAD) -o

OBJ=obj/reagehttp.o obj/config.o obj/config.o obj/base64.o obj/buffer.o obj/pool.o \
	obj/http_mod_authorized.o obj/http_mod_connect.o \
	obj/http_request.o obj/http_send_page.o obj/http_virtual_port.o \
	obj/http_send_html.o obj/http_autoindex.o obj/http_mod_cgi.o obj/http_deamon.o \
	obj/linux_epoll.o obj/str.o obj/hash.o

DST=obj/

obj/reagehttp:$(OBJ)
	gcc  $(CFLAGS) $@ $^ 


$(DST)%.o:http/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

$(DST)%.o:core/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

$(DST)%.o:linux/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

clean:
	rm -rf */*.o
















