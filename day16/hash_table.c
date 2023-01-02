#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash_table.h"


#ifndef HT_CAPACITY
    #include <limits.h>
    #define HT_CAPACITY 100000000
#endif


// TODO: Add documentation
unsigned long long get_hash(char *key) {
    unsigned int key_len = strlen(key);
    unsigned long long hash_value = 0;
    for (unsigned int index = 0; index < key_len; index++) {
        hash_value *= 100;
        hash_value += key[index];
    }
    return hash_value;
}


// TODO: Add documentation
hash_table_t *create_table(unsigned long table_size) {
    hash_table_t *new_table = malloc(sizeof(hash_table_t));
    new_table->size = table_size;
    new_table->count = 0;
    new_table->items = malloc(HT_CAPACITY * sizeof(void *));
    for (unsigned long index = 0; index < table_size; index++) {
        new_table->items[index] = NULL;
    }
    new_table->hashes = malloc(sizeof(unsigned long long));
    return new_table;
}


// TODO: Add documentation
hash_info_item_t * create_info_item(char *key, int pressure, char **connections, unsigned int num_connections) {
    
    // Allocate memory for item and members
    hash_info_item_t *new_item = malloc(sizeof(hash_info_item_t));
    new_item->key = malloc(strlen(key) + 1);
    new_item->rate = pressure;
    new_item->opened = false;
    new_item->num_connections = num_connections;
    new_item->connections = malloc(num_connections * sizeof(char *));

    // Load string data into the connections
    strncpy(new_item->key, key, strlen(key) + 1);
    for (unsigned int index = 0; index < num_connections; index++) {
        char *cpy_str = connections[index];
        unsigned int cpy_len = strlen(cpy_str) + 1;
        new_item->connections[index] = malloc(cpy_len);
        strncpy(new_item->connections[index], cpy_str, cpy_len);
    }

    return new_item;

}


// TODO: Add documentation
hash_route_item_t * create_route_item(char *key_a, char *key_b, unsigned int duration) {

    // Allocate memory for item and members
    hash_route_item_t *new_item = malloc(sizeof(hash_route_item_t));
    new_item->key_a = malloc(strlen(key_a) + 1);
    new_item->key_b = malloc(strlen(key_b) + 1);
    new_item->duration = duration;

    // Load string data into the connections
    strncpy(new_item->key_a, key_a, strlen(key_a) + 1);
    strncpy(new_item->key_b, key_b, strlen(key_b) + 1);

    return new_item;

}


// TODO: Add documentation
void insert_item(hash_table_t *table, void *new_item, unsigned long long hash) {

    // For the sake of this table, assume there won't be collsions
    if (table->items[hash] != NULL) {
        printf("Key already exists in table!  Exiting...\n");
        exit(1);
    }

    // Check if table is already full
    if (table->count == table->size) {
        printf("Table is full!  Exiting...\n");
        exit(1);
    }

    // Add the item to the table
    table->items[hash] = new_item;
    table->count++;

    // Add the new hash to the list of hashes
    table->hashes = realloc(table->hashes, table->count * sizeof(unsigned long long));
    table->hashes[table->count - 1] = hash;

}


// TODO: Add documentation
void insert_info_item(hash_table_t *table, char *key, int pressure, char **connections, unsigned int num_connections) {
    hash_info_item_t *new_item = create_info_item(key, pressure, connections, num_connections);
    unsigned long long new_item_hash = get_hash(key);
    insert_item(table, new_item, new_item_hash);
}


// TODO: Add documentation
void insert_route_item(hash_table_t *table, char *key_a, char *key_b, unsigned int duration) {
    hash_route_item_t *new_item = create_route_item(key_a, key_b, duration);
    unsigned long long hash_a = get_hash(key_a);
    unsigned long long hash_b = get_hash(key_b);
    unsigned long long new_item_hash = hash_a * hash_b;
    insert_item(table, new_item, new_item_hash);
}


// TODO: Add documentation
hash_info_item_t * retrieve_info_by_key(hash_table_t table, char *key) {
    unsigned long long hash_value = get_hash(key);
    return (hash_info_item_t *)retrieve_by_hash(table, hash_value);
}


// TODO: Add documentation
hash_route_item_t * retrieve_route_by_keys(hash_table_t table, char *key_a, char *key_b) {
    unsigned long long hash_a = get_hash(key_a);
    unsigned long long hash_b = get_hash(key_b);
    unsigned long long hash_value = hash_a * hash_b;
    return (hash_route_item_t *)retrieve_by_hash(table, hash_value);
}


// TODO: Add documentation
void * retrieve_by_hash(hash_table_t table, unsigned long hash_value) {
    void *req_item = table.items[hash_value];
    if (req_item == NULL) { return NULL; }
    return req_item;
}
