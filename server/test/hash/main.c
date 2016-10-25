#include <stdio.h>

#include "../../core/pool.h"
#include "../../core/hash.h"
#include "../../core/str.h"

// gcc  -g -o hash main.c ../../core/pool.c  ../../core/hash.c  ../../core/str.c


typedef struct hash_test {
    int i;
    char *p;
}hash_test_t;

void print_hash(char *info, hash_t *h) {
    int i = 0;
    hash_item_t * item;
    hash_test_t * th;

    printf("\n -----%s hash size %d count %d \n", info, h->size, h->count);

    for(i = 0; i < h->size; i++) {
        item = h->buckets[i].items;
        while(item) {
            switch(item->type) {
                case HASH_ITEM_VALUE_TYPE_INT:
                    printf("%s int  %d\n", item->key->ptr, item->value.i);
                break;
                case HASH_ITEM_VALUE_TYPE_LONG:
                    printf("%s long  %ld\n", item->key->ptr, item->value.l);
                break;
                case HASH_ITEM_VALUE_TYPE_DOUBLE:
                    printf("%s double %f\n", item->key->ptr, item->value.d);
                break;
                case HASH_ITEM_VALUE_TYPE_STRING:
                    printf("%s string %s \n", item->key->ptr, item->value.str->ptr);
                break;
                case HASH_ITEM_VALUE_TYPE_PTR:
                    th = (hash_test_t *)item->value.ptr;
                    printf("%s ptr %d - %s\n", item->key->ptr, th->i, th->p);
                    break;
                default:
                    break;
            }
            item = item->next;

        }

    }
    printf("end -----------\n");
}

int main() {
    pool_t *p = pool_create();
    char a[] = "str hash";

    int i = 0;
    long l = 10;
    double d = 10.0f;
    int ii = -1;
    int ll = -1;
    double dd = 0.0f;



    string *str = string_init_from_str(p, a, strlen(a));
    string *tstr = string_init(p);
    hash_test_t ht ;
    hash_test_t tht ;
    ht.i = 100;
    ht.p  = a;

    string *key = string_init_from_str(p, "key", 3);
    string *key1 = string_init_from_str(p, "key1", 4);
    string *key2 = string_init_from_str(p, "kkkkk", 5);
    string *strValue2 = string_init_from_str(p, "sdafsdfasdf", 11);
    hash_t *h = (hash_t *)hash_init(p);

    hash_add_int(p, h, key->ptr, key->len, 1);
    hash_add_long(p, h, key1->ptr, key1->len, 100);
    hash_add_string(p, h, key2->ptr, key2->len, strValue2);

    hash_get_int(h, key->ptr, key->len, &ii);
    print_hash("int", h);


    hash_add_long(p, h, key->ptr, key->len, l);
    hash_get_long(h, key->ptr, key->len, &ll);
    print_hash("long", h);



    hash_add_double(p, h, key->ptr, key->len, d);
    hash_get_double(h, key->ptr, key->len, &dd);
    print_hash("double", h);



    hash_add_string(p, h, key->ptr, key->len, str);
    hash_get_string(h, key->ptr, key->len, tstr);
    print_hash("string", h);


    hash_add_ptr(p, h, key->ptr, key->len, (void *)&ht);
    hash_get_ptr(h, key->ptr, key->len, &tht,sizeof(hash_test_t));
    print_hash("ptr", h);

    hash_add_int(p, h, "B", 1, 1000);
    print_hash("add del 2item", h);
    
    hash_del_item(h, key1->ptr, key1->len);
    print_hash("del", h);

    hash_add_int(p, h, "J", 1, 1000);
    hash_add_int(p, h, "a", 1, 1000);
    hash_add_int(p, h, "b", 1, 1000);
    hash_add_int(p, h, "c", 1, 1000);
    hash_add_int(p, h, "d", 1, 1000);
    hash_add_int(p, h, "e", 1, 1000);
    hash_add_int(p, h, "f", 1, 1000);
    hash_add_int(p, h, "g", 1, 1000);
    hash_add_int(p, h, "b", 1, 1000);
    hash_add_int(p, h, "b", 1, 1000);
    hash_add_int(p, h, "b", 1, 1000);
    
    print_hash("rehash", h);

    i ++;


}