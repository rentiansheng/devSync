#include "hash.h"

hash_t * hash_init(pool_t *p) {
    hash_t *h = NULL;

    h =(hash_t  *) palloc(p, sizeof(hash_t));
    h->buckets = NULL;
    h->size = HASH_DEFAULT_SIZE;
    h->count = 0;
    h->mask = h->size - 1;
    h->buckets = (bucket_t *)pcalloc(p, sizeof(bucket_t)*h->size); 

    return h;
}



int hash_add_int(pool_t *p, hash_t  *h, const char  *key, int keyLen, int  v) {
    return hash_add(p, h ,key, keyLen, &v, HASH_ITEM_VALUE_TYPE_INT);
}

int hash_add_long(pool_t *p, hash_t *h, const char *key, int keyLen, long int  v) {
    return hash_add(p, h ,key, keyLen, &v, HASH_ITEM_VALUE_TYPE_LONG);
}

int hash_add_double(pool_t *p, hash_t *h, const char *key, int keyLen, double v) {
    return hash_add(p, h ,key, keyLen, &v, HASH_ITEM_VALUE_TYPE_DOUBLE);
}

int hash_add_string(pool_t *p, hash_t *h, const char *key, int keyLen, string *v) {
    return hash_add(p, h ,key, keyLen, v, HASH_ITEM_VALUE_TYPE_STRING);
}

int hash_add_ptr(pool_t *p, hash_t *h, const char *key, int keyLen, void * v) {
     return hash_add(p, h ,key, keyLen, v, HASH_ITEM_VALUE_TYPE_PTR);
}

int hash_get_int(hash_t *h, const char *key, int keyLen,int * ret) {

    hash_item_t * item  = get_hash_item(h, key, keyLen);

    if(item == NULL || item->type != HASH_ITEM_VALUE_TYPE_INT) {
        return -1;
    }

    *ret = item->value.i;

    return 0;
}

int hash_get_long(hash_t *h, const char *key, int keyLen, long * ret) {
    hash_item_t * item  = get_hash_item(h, key, keyLen);

    if(item == NULL || item->type != HASH_ITEM_VALUE_TYPE_LONG) {
        return -1;
    }

    *ret = item->value.l;

    return 0;
}

int hash_get_double(hash_t *h, const char *key, int keyLen, double * ret) {
     hash_item_t * item  = get_hash_item(h, key, keyLen);

    if(item == NULL || item->type != HASH_ITEM_VALUE_TYPE_DOUBLE) {
        return -1;
    }

    *ret = item->value.d;

    return 0;
}

int hash_get_string(hash_t *h, const char *key, int keyLen, string * ret) {
    hash_item_t * item  = get_hash_item(h, key, keyLen);

    if(item == NULL || item->type != HASH_ITEM_VALUE_TYPE_STRING) {
        return -1;
    }

    memcpy(ret, item->value.str, sizeof(string));

    return 0;
}

int hash_get_ptr(hash_t *h, const char *key, int keyLen, void * ret, size_t size) {
     hash_item_t * item  = get_hash_item(h, key, keyLen);

    if(item == NULL || item->type != HASH_ITEM_VALUE_TYPE_PTR) {
        return -1;
    }

    memcpy(ret, item->value.ptr, size);

    return 0;
}

int hash_del_item(hash_t *h, const char *key, int keyLen) {

    int index = hash_index(h, key, keyLen);
    hash_item_t *pre, *cur ;
    pre = NULL;
    cur = h->buckets[index].items;
    if(h->buckets[index].count == 0 || cur == NULL) {
        return 0;
    }
    while(cur) {
        if(keyLen == cur->key->len && strncmp(key, cur->key->ptr, keyLen) == 0) {
            break;
        }
        pre = cur;
        cur = cur->next;
    }
    if(cur != NULL) {
        if(pre == NULL) {
            h->buckets[index].items = cur->next;
        } else {
            pre->next = cur->next;
        }
        h->buckets[index].count--;
    }
    

    return 0;
}

hash_item_t * get_hash_item(hash_t *h, const char *key, int keyLen) {
    unsigned int index = hash_index(h, key, keyLen);

    hash_item_t * item = h->buckets[index].items;
    if(h->buckets[index].count == 0 || item == NULL) {
        return NULL;
    }

    while(item ) {
        if(strncmp(key, item->key->ptr, keyLen) == 0) {
            return item;
        }
        item = item->next;
    }

    return NULL;

}

static int hash_add(pool_t *p, hash_t *h ,const char * key, int keyLen, void * ptr, int type) {

    hash_item_t * newItem = (hash_item_t *)pcalloc(p, sizeof(hash_item_t));
    newItem->key = string_init_from_ptr(p, key, keyLen);
    newItem->type = type;
    newItem->next = NULL;

    if((h->count+1) > h->size) {
        int reint = hash_reinit(p, h);
        if(reint) {
            return reint;
        }
    }

    switch(type) {
        case HASH_ITEM_VALUE_TYPE_INT:
        newItem->value.i = *(int*)ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_LONG:
        newItem->value.l = *(long int*)ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_DOUBLE:
        newItem->value.d = *(double*)ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_STRING:
        newItem->value.str = (string *) ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_PTR:
        newItem->value.ptr = (void *) ptr;
        break;
        default:
        return -1;
    }

    hash_add_item(h, newItem);


    return 0;

}

int hash_add_item(hash_t *h, hash_item_t *newItem) {

    unsigned int index = hash_index(h, newItem->key->ptr, newItem->key->len);


    hash_item_t *i = (h->buckets[index]).items;
    int isAdd = 1;
    while(i) {
        if(string_compare(i->key, newItem->key) == 0) {
            isAdd = 0;
            i->type = newItem->type;
            i->value = newItem->value;
            break;
        }
        i = i->next;
    }
    if(isAdd) {
        (h->buckets[index]).count += 1;
        h->count += 1;
        newItem->next = h->buckets[index].items;
        h->buckets[index].items = newItem;
    }
        
    return 0;
}

static unsigned int  hash_reinit(pool_t *p, hash_t *h) {

    hash_t *rehash = NULL;
    int i = 0;
    hash_item_t * item;
    hash_item_t *next;

    rehash =(hash_t  *) palloc(p, sizeof(hash_t));
    rehash->buckets = NULL;
    rehash->size = (h->size)<<1;
    rehash->count = 0;
    rehash->mask = rehash->size - 1;
    rehash->buckets = (bucket_t *)pcalloc(p, sizeof(bucket_t)*rehash->size);

    for(i = 0; i < h->size; i++){
        item = h->buckets[i].items;
        while(item) {
            next = item->next;
            hash_add_item(rehash, item);
            item = next;
        }
    }

    memcpy(h, rehash, sizeof(hash_t));

    return 0;
}

static unsigned int hash_index(hash_t * h, char *ptr, int len) {
    unsigned int index = 0;
    int i = 0;
    while(i < len) {
        index = index + (ptr[i]<<5) + ptr[i];
        i++;
    }

    index = index&h->mask;
    
    return index;

}


