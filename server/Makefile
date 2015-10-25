all:obj/reagehttp



MYHEAD = -I core/ -I http/
CFLAGS = -g -W -Wall $(MYHEAD) -o

OBJ=obj/reagehttp.o obj/config.o obj/config.o obj/base64.o obj/buffer.o obj/pool.o \
	obj/http_mod_connect.o obj/http_file_write.o obj/http_send_page.o obj/http_request.o
DST=obj/

obj/reagehttp:$(OBJ)
	gcc  $(CFLAGS) $@ $^ 


$(DST)%.o:http/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^

$(DST)%.o:core/%.c
	echo $^
	gcc -c $(CFLAGS) $@ $^


clean:
	rm -rf */*.o
















