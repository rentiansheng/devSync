/*
 *Copyright (c) Reage
 *blog:http:www.rhttp.cn
 */

#include "base64.h"


static int encode_base64(const read_buffer *src, buffer *dst);
static int decode_base64(const read_buffer *src, buffer *dst);

int
read_buffer_move_word(read_buffer * b, char *word)
{
	int len = strlen(word);
	if(strncasecmp(b->ptr, word, len) == 0) {
		b->ptr += len;
		b->size -= len ;

		while(*b->ptr == ' ') {
			b->ptr++;
			b->size--;
		}

		return 0;
	}

	return 1;
}

int 
decoded_usr_pwd(http_connect_t *con)
{
	pool_t *p;
	buffer *dst;
	request *in;
	read_buffer *base64code;
	int len;

	in = con->in;
	if(in->authorization ==  NULL)return 0;

	p = (pool_t *)con->p;
	base64code = in->authorization;
	if( read_buffer_move_word(base64code, "basic") == 0 ) {

		len = 3*(base64code->size/4) + 3; 
		dst = (buffer *)buffer_create_size(p, len);
		in->user = palloc(p, sizeof(read_buffer));
		in->pwd = palloc(p, sizeof(read_buffer));

		if(decode_base64(base64code, dst) == 0){
			dst->size = dst->used;
			dst->used = 0;
			buffer_get_word_with_split(dst, in->user, ':');
			buffer_get_line(dst, in->pwd);
		}

	}

	return 0;
}


static int 
decode_base64(const read_buffer  *rbsrc, buffer *bdst)
{
	static char base64char[] ={
		-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
		-1 ,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
		-1 , 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	};

	int i;
	char *src, *dst;
	size_t len = rbsrc->size;
	
	src  = rbsrc->ptr;
	dst = bdst->ptr;
	for(i = 0; i < len; i++){
		if(src[i] == '='){
			len = i;
		}
		else if(base64char[src[i]] == -1){
			return 1; //unexpected characters
		}
	}

	if(len % 4 == 1){
		return  2; 
	}
	
	bdst->used = 3*(len/4) + ((len%4)>2?2:1);
	if((len%4) == 0) bdst->used--;
	if(bdst->used > bdst->size) return -3;
	while(len > 3){
		*dst++ = (char)(base64char[src[0]]<<2) | (base64char[src[1]]>>4 & 0x3);
		*dst++ = (char)(base64char[src[1]]<<4 )|(base64char[src[2]]>>2 & 0xf);
		*dst++ = (char)(base64char[src[2]]<<6)|(base64char[src[3]]);
		
		src += 4;
		len -= 4;
	}

	if(len){
 		if(len > 1){
			*dst++ = (char)(base64char[src[0]]<<2) | (base64char[src[1]]>>4 & 0x3);
		}

		if(len > 2){
			*dst++ = (char)(base64char[src[1]]<<4 )|(base64char[src[2]]>>2 & 0xf);
		}
 
	}

	return 0;
} 


static int 
encode_base64(const read_buffer *bsrc, buffer *bdst)
{
	char *src = bsrc->ptr;
	size_t len = strlen(src); 
	static char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char *dst = bdst->ptr;

	while(len > 2){
		*dst++  = base64char[src[0] >> 2 & 0x3f];
		*dst++ = base64char[(src[0] & 0x3)<<4 | src[1] >> 4 & 0xf];
		*dst++ = base64char[(src[1] & 0xf)<<2 | src[2]>>6 & 0x3];
		*dst++ = base64char[src[2] & 0x3f];
		len -= 3;
		src += 3;
	}

	if(len){
		*dst++ = base64char[src[0] >> 2 & 0x3f];
		if(len > 1){
			*dst++ = base64char[((src[0] & 0x3)<<4) | ((src[1]>> 4) & 0xf) ];
			*dst ++ = base64char[(src[1] & 0xf)<<2];
		}
		else{
			*dst++ = base64char[(src[0] & 0x3) << 4];
			*dst++ = '=';
		}
		*dst++  = '=';
	}
	
	*dst = 0;

	return 0; 
}

