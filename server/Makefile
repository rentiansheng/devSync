all:obj/devSync



MYHEAD = -I core/ -I http/ -I http/modules -I linux
CFLAGS = -g -W -Wall $(MYHEAD) -o

OBJ=obj/devsync.o obj/config.o obj/config.o obj/base64.o obj/buffer.o obj/str.o obj/pool.o \
	obj/http_mod_connect.o obj/http_file_write.o obj/http_send_page.o obj/http_request.o obj/ds_daemon.o \
	obj/ds_log.o obj/http_header.o obj/linux_epoll.o

DST=obj/

obj/devSync:$(OBJ)
	echo $^
	gcc  $(CFLAGS) $@ $^ 


$(DST)%.o:http/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

$(DST)%.o:core/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

$(DST)%.o:http/modules/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

$(DST)%.o:linux/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

clean:
	rm -rf */*.o
	rm obj/devSync
















