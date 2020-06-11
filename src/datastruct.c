// ------------------------------------------------------------------------------------------------------------
// CiteSoft
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


//Frees all memory associated with a citation entry
//Version is not freed, since it is simply a pointer to an item contained in the field list(if present)
void destroyCitation(citation_entry_t* entry)
{
    free(entry->uniqueID);
    free(entry->softwareName);
    free(entry->timestamp);
    destroyFieldList(entry->fields);
    free(entry);
}

//Creates and return a deep copy of a linked list of fields
field_t* copyFieldList(field_t* firstEntry)
{
    if(!firstEntry)//If the first pointer is null, there is no list to copy
    {
        return NULL;
    }
    field_t* returnVal = copyField(firstEntry);
    field_t* newFieldPointer = returnVal;
    field_t* oldFieldPointer = firstEntry->nextField;
    while(oldFieldPointer)//While there's another field to copy
    {
        newFieldPointer->nextField = copyField(oldFieldPointer);//Link current end of return list to new field
        newFieldPointer = newFieldPointer->nextField;//Move pointer to the end of the return list
        oldFieldPointer = oldFieldPointer->nextField;//Advance pointer on argument list
    }
    return returnVal;
}

//Creates and return a deep copy of a linked list of const_fields(fields which point to unmutable strings)
field_t* createFieldList(const_field_t* firstEntry)
{
    if(!firstEntry)//If the first pointer is null, there is no list to copy
    {
        return NULL;
    }
    field_t* returnVal = createField(firstEntry);
    field_t* newFieldPointer = returnVal;
    const_field_t* oldFieldPointer = firstEntry->nextField;
    while(oldFieldPointer)//While there's another field to copy
    {
        newFieldPointer->nextField = createField(oldFieldPointer);//Link current end of return list to new field
        newFieldPointer = newFieldPointer->nextField;//Move pointer to the end of the return list
        oldFieldPointer = oldFieldPointer->nextField;//Advance pointer on argument list
    }
    return returnVal;
}

//Frees all memory associated with a linked list of fields
void destroyFieldList(field_t* firstEntry)
{
    field_t* fieldPtr = firstEntry;
    field_t* nextPtr;
    while(fieldPtr)//While there is still an item in the list
    {
        //Loop through and delete every item in the list's array
        for(int i = 0; i < fieldPtr->numOfValues; i++)
        {
            free(fieldPtr->fieldValue[i]);
        }
        free(fieldPtr->fieldValue);//Free the array pointer
        free(fieldPtr->fieldName);
        nextPtr = fieldPtr->nextField;//Store a link to the next item in the list
        free(fieldPtr);//Free the field itself
        fieldPtr = nextPtr;//Advance to next item in the list, if present
    }
}

//Creates and returns a deep copy of a single field
field_t* copyField(field_t* fieldToCopy)
{
    field_t *returnVal = malloc(sizeof(field_t));
    returnVal->nextField = NULL;
    returnVal->fieldName = malloc(sizeof(char) * strlen(fieldToCopy->fieldName) + 1);//Allocate memory for the field name
    strcpy(returnVal->fieldName, fieldToCopy->fieldName);//Copy the field name
    returnVal->numOfValues = fieldToCopy->numOfValues;
    returnVal->fieldValue = (char**) malloc(sizeof(char*) * returnVal->numOfValues);//Allocate enough memory for array of pointers to strings
    for(int i = 0; i < returnVal->numOfValues; i++)
    {
        //For every string in the argument field's array, allocate memory for the string, save the pointer to the return struct, then copy the string
        int len = strlen(fieldToCopy->fieldValue[i]) + 1;//
        returnVal->fieldValue[i] = (char*) malloc(sizeof(char) * len);
        strcpy(returnVal->fieldValue[i], fieldToCopy->fieldValue[i]);
    }
    return returnVal;
}

//Creates and returns a deep copy of a single field with pointers to unmutable strings
field_t* createField(const_field_t* fieldToCreate)
{
    field_t *returnVal = malloc(sizeof(field_t));
    returnVal->nextField = 0;
    returnVal->fieldName = malloc(sizeof(char) * strlen(fieldToCreate->fieldName) + 1);//Allocate memory for the field name
    strcpy(returnVal->fieldName, fieldToCreate->fieldName);//Copy the field name
    returnVal->numOfValues = fieldToCreate->numOfValues;
    returnVal->fieldValue = (char**) malloc(sizeof(char*) * returnVal->numOfValues);//Allocate enough memory for array of pointers to strings
    for(int i = 0; i < returnVal->numOfValues; i++)
    {
        //For every string in the argument field's array, allocate memory for the string, save the pointer to the return struct, then copy the string
        int len = strlen(fieldToCreate->fieldValue[i]) + 1;
        returnVal->fieldValue[i] = (char*) malloc(sizeof(char) * len);
        strcpy(returnVal->fieldValue[i], fieldToCreate->fieldValue[i]);
    }
    return returnVal;
}

//Returns a pointer to a mutable copy of oldStr
//Caller is responsible for freeing this pointer once it is no longer needed
char* allocateAndCopyStr(const char* oldStr)
{
    int size = strlen(oldStr) + 1;
    char* returnVal = (char *)malloc(sizeof(char) * size);
    strcpy(returnVal, oldStr);
    return returnVal;
}
