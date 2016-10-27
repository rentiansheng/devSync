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

	len = s1->len > s2->len?s2->len:s1->len;
	if(len > 1) len--;

	return (s1->ptr[0] - s2->ptr[0]);
	
}

void string_copy_str_n(const string *b, char *s1, int n){
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