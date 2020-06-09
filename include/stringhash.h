#ifndef STRINGHASH_H
#define STRINGHASH_H
#include "datastruct.h"

#define HASH_TABLE_SIZE 1024

typedef struct item_list_t
{
    citation_entry_t *value;
    struct item_list_t *nextItem;
} item_list_t;

typedef struct hash_table_item_t
{
    citation_entry_t *value;
    struct hash_table_item_t *nextEntry;
} hash_table_item_t;

typedef struct string_hash_table_t
{
    hash_table_item_t *dataArray[HASH_TABLE_SIZE];
} string_hash_table_t;


void addItem(string_hash_table_t* hashTable, citation_entry_t *item, citation_entry_t* (*resolveCollision)(citation_entry_t*, citation_entry_t*));

//Finds an item with the specified key(if present) and returns a pointer to it
//The item remains in the table
citation_entry_t* getItem(string_hash_table_t * hashTable, char* key);

//Finds an item with the specified key(if present) and removes it from the table, then returns the item
//Returns a null pointer if an item with a matching key cannot be found
citation_entry_t* removeItem(string_hash_table_t* hashTable, char* key);

//Return all items as a linked list(without freeing the table)
//Designed to allow users to iterate through the entire table
//Must call destroyList after completion
item_list_t* getAllItems(string_hash_table_t *hashTable);

//Frees the memory allocated for a list
//Does not free any items
void destroyList(item_list_t* itemList);

//Frees all memory allocated by the list, including memory used by items
void destroyTable(string_hash_table_t* hashTable, void (*destroyItem)(citation_entry_t*));

int hashFunction(char *key);

#endif
