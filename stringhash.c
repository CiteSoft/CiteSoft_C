// ------------------------------------------------------------------------------------------------------------
// Cite Soft
//
// C
//
// Developer : CPH
// Date      : 06-03-2020
//
// ------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "datastruct.h"
#include "stringhash.h"


//Resolve collision should accept a pointer to two entries, return the entry to keep, and destroy the other entry(otherwise, memory leak results)
void addItem(string_hash_table_t* hashTable, citation_entry_t *item, citation_entry_t* (*resolveCollision)(citation_entry_t*, citation_entry_t*))
{
    int hash = hashFunction(item->uniqueID);
    hash_table_item_t* newItem = (hash_table_item_t*) malloc(sizeof(hash_table_item_t));
    newItem->value = item;
    newItem->nextEntry = 0;
    if(!hashTable->dataArray[hash])
    {
        hashTable->dataArray[hash] = newItem;
    }
    else
    {
        hash_table_item_t *itemPtr = hashTable->dataArray[hash];
        while(itemPtr->nextEntry)
        {
            if(!strcmp(itemPtr->value->uniqueID, item->uniqueID))
            {
                itemPtr->value = resolveCollision(itemPtr->value, item);
                free(newItem);//New item wasn't needed, free it
                return;//New value is now in table, return
            }
            itemPtr = itemPtr->nextEntry;
        }
        itemPtr->nextEntry = newItem;
    }
}

//Finds an item with the specified key(if present) and returns a pointer to it
//The item remains in the table
citation_entry_t* getItem(string_hash_table_t * hashTable, char* key)
{
    int hash = hashFunction(key);
    hash_table_item_t *itemPtr = hashTable->dataArray[hash];
    while(itemPtr)
    {
        if(!strcmp(itemPtr->value->uniqueID, key))
        {
            //Strings match
            itemPtr->value;
        }
        itemPtr = itemPtr->nextEntry;
    }
    return NULL;//Item wasn't found, return null
}

//Finds an item with the specified key(if present) and removes it from the table, then returns the item
//Returns a null pointer if an item with a matching key cannot be found
citation_entry_t* removeItem(string_hash_table_t* hashTable, char* key)
{
    int hash = hashFunction(key);
    hash_table_item_t *itemPtr = hashTable->dataArray[hash];
    if(itemPtr == 0)
    {
        return NULL;//Key doesn't exist
    }
    else if(itemPtr->nextEntry == 0)
    {
        if(!strcmp(itemPtr->value->uniqueID, key))
        {
            //Strings match
            citation_entry_t* returnVal = itemPtr->value;
            hashTable->dataArray[hash] = 0;
            free(itemPtr);
            return returnVal;
        }
    }
    else
    {
        hash_table_item_t *lastItemPtr = itemPtr;
        itemPtr = itemPtr->nextEntry;
        while(itemPtr)
        {
            if(!strcmp(itemPtr->value->uniqueID, key))
            {
                //Strings match
                citation_entry_t* returnVal = itemPtr->value;
                lastItemPtr->nextEntry = itemPtr->nextEntry;
                free(itemPtr);
                return returnVal;
            }
            else
            {
                //Otherwise, search next item
                lastItemPtr = itemPtr;
                itemPtr = itemPtr->nextEntry;
            }
        }
    }
    return NULL;//Item couldn't be found, return error
}

//Return all items as a linked list(without freeing the table)
//Designed to allow users to iterate through the entire table
//Must call destroyList after completion
item_list_t* getAllItems(string_hash_table_t *hashTable)
{
    item_list_t *returnList = NULL;
    item_list_t *currentItem = NULL;
    for(int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hash_table_item_t *hashItem = hashTable->dataArray[i];
        while(hashItem)
        {
            item_list_t *newItem = (item_list_t*) malloc(sizeof(item_list_t));
            newItem->value = hashItem->value;
            newItem->nextItem = 0;
            if(currentItem == NULL)
            {
                returnList = newItem;
                currentItem = newItem;
            }
            else
            {
                currentItem->nextItem = newItem;
                currentItem = newItem;
            }
            hashItem = hashItem->nextEntry;
        }
    }
    return returnList;
}

//Frees the memory allocated for a list
//Does not free any items
void destroyList(item_list_t* itemList)
{
    item_list_t* ptr = itemList;
    item_list_t* nextPtr;
    while(ptr)
    {
        nextPtr = ptr->nextItem;
        free(ptr);
        ptr = nextPtr;
    }
}

//Frees all memory allocated by the list, including memory used by items
void destroyTable(string_hash_table_t* hashTable, void (*destroyItem)(citation_entry_t*))
{
    for(int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hash_table_item_t *item = hashTable->dataArray[i];
        while(item)
        {
            destroyItem(item->value);
            hash_table_item_t *nextItem = item->nextEntry;
            free(item);
            item = nextItem;
        }
    }
}

int hashFunction(char *key)
{
    return 1;
}
