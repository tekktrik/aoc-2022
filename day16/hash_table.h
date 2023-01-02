#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

// TODO: Add documentation
typedef struct HashInfoItem {
    char *key;
    int rate;
    char **connections;
    unsigned int num_connections;
    bool opened;
} hash_info_item_t;


// TODO: Add documentation
typedef struct HashRouteItem {
    char *key_a;
    char *key_b;
    unsigned int duration;
} hash_route_item_t;


// TODO: Add documentation
typedef struct HashTable {
    void **items;
    unsigned long size;
    unsigned long count;
    unsigned long long *hashes;
} hash_table_t;

unsigned long long get_hash(char *key);
hash_table_t * create_table(unsigned long table_size);
hash_info_item_t * create_info_item(char *key, int pressure, char **connections, unsigned int num_connections);
hash_route_item_t * create_route_item(char *key_a, char *key_b, unsigned int duration);
void insert_item(hash_table_t *table, void *new_item, unsigned long long hash);
void insert_info_item(hash_table_t *table, char *key, int pressure, char **connections, unsigned int num_connections);
void insert_route_item(hash_table_t *table, char *key_a, char *key_b, unsigned int duration);
hash_info_item_t * retrieve_info_by_key(hash_table_t table, char *key);
hash_route_item_t * retrieve_route_by_keys(hash_table_t table, char *key_a, char *key_b);
void * retrieve_by_hash(hash_table_t table, unsigned long hash_value);

#endif // HASH_TABLE_H