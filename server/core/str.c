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
	strncpy(str->ptr, b, len);
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