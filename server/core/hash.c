#include "hash.c"

hash * hash_init(pool_t *p) {
    hash *h = NULL;

    h =(hash  *) palloc(p, sizeof(hash));
    h->buckets = NULL;
    h->size = HASH_DEFAULT_SIZE;
    h->count = 0;
    h->mask = h->size - 1;
    h->buckets = (bucket *)pcalloc(p, sizeof(bucket)); 

    return h;
}



int hash_add_int(pool_t *p, const *key, int keyLen, int  v) {
}

static int hash_add(pool_t *p, hash *h ,const char * key, int keyLen, void * ptr, int type) {
    unsigned int index = hash_index(char *key, keyLen);

    item * newItem = (item *)pcalloc(p, sizeof(item));
    newItem->key = string_init_from_ptr(key, keyLen);
    newItem->type = type;
    newItem->next = NULL;

    switch(type) {
        case HASH_ITEM_VALUE_TYPE_INT:
        newItem->value.i = *(int*)ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_LONG:
        newItem->value.l = *(long int)ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_DOUBLE:
        newItem->v.d = *(long int)ptr;
        break;
        case HASH_ITEM_VALUE_TYPE_STRING:
        newItem->value.str = (string *) ptr;
        break;
        default:
        return -1;
    }

    
    if(h->count/h->size) {
        int reint = hash_reinit(pool_t *p, hash *h);
        if(reint) {
            return reint;
        }
    }
    index = index&h->mask;

    if((h->buckets[index]).count == 0) {

        (h->buckets[index]).items = newItem;
    }else {
        item i = (h->buckets[index]).item;
        while(i->next != null) {
            i = i->next;
        }
        i->next = newItem;
    }
    (h->buckets[index]).count += 1;
    h->count += 1;


    return 0;

}

static int hash_add_item(pool_t *p, hash *h, item *i) {
    unsigned int index = hash_init(i->key.ptr, i->key.len);
    
    index = index&h->mask;


}

static unsigned int  hash_reinit(pool_t *p, hash *h) {

    hash *rehash = NULL;
    int i = 0;
    item * hash_item;

    rehash =(hash  *) palloc(p, sizeof(hash));
    rehash->buckets = NULL;
    rehash->size = (rehash->size)<<1;
    rehash->count = 0;
    rehash->mask = rehash->size - 1;
    rehash->buckets = (bucket *)pcalloc(p, sizeof(bucket));

    for(i = 0; i < h.count; i++){
        hash_item = h->bucket[i];
        while(hash_item != NULL) {
            hash_add_item(p, rehash, hash_item);
            hash_item = hash_item->next;
        }
    }

    memcpy(h, rehash, sizeof(hash));

    return h;
}




static unsigned int hash_index(char *ptr, int len) {
    unsigned int index = 0;
    int i = 0;
    while(i < len) {
        index = index + (ptr[i]<<5) + ptr[i];
    }
    
    return index;

}
