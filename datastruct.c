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



void destroyCitation(citation_entry_t* entry)
{
    free(entry->uniqueID);
    free(entry->softwareName);
    free(entry->timestamp);
    destroyFieldList(entry->fields);
    free(entry);
}

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
        newFieldPointer->nextField = copyField(oldFieldPointer);
        newFieldPointer = newFieldPointer->nextField;
        oldFieldPointer = oldFieldPointer->nextField;
    }
    return returnVal;
}

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
        newFieldPointer->nextField = createField(oldFieldPointer);
        newFieldPointer = newFieldPointer->nextField;
        oldFieldPointer = oldFieldPointer->nextField;
    }
    return returnVal;
}

void destroyFieldList(field_t* firstEntry)
{
    field_t* fieldPtr = firstEntry;
    field_t* nextPtr;
    while(fieldPtr)
    {
        for(int i = 0; i < fieldPtr->numOfValues; i++)
        {
            free(fieldPtr->fieldValue[i]);
        }
        free(fieldPtr->fieldValue);
        free(fieldPtr->fieldName);
        nextPtr = fieldPtr->nextField;
        free(fieldPtr);
        fieldPtr = nextPtr;
    }
}

field_t* copyField(field_t* fieldToCopy)
{
    field_t *returnVal = malloc(sizeof(field_t));
    returnVal->nextField = 0;
    returnVal->fieldName = malloc(sizeof(char) * strlen(fieldToCopy->fieldName) + 1);
    strcpy(returnVal->fieldName, fieldToCopy->fieldName);
    returnVal->numOfValues = fieldToCopy->numOfValues;
    returnVal->fieldValue = (char**) malloc(sizeof(char*) * returnVal->numOfValues);
    for(int i = 0; i < returnVal->numOfValues; i++)
    {
        int len = strlen(fieldToCopy->fieldValue[i]) + 1;
        returnVal->fieldValue[i] = (char*) malloc(sizeof(char) * len);
        strcpy(returnVal->fieldValue[i], fieldToCopy->fieldValue[i]);
    }
    return returnVal;
}

field_t* createField(const_field_t* fieldToCreate)
{
    field_t *returnVal = malloc(sizeof(field_t));
    returnVal->nextField = 0;
    returnVal->fieldName = malloc(sizeof(char) * strlen(fieldToCreate->fieldName) + 1);
    strcpy(returnVal->fieldName, fieldToCreate->fieldName);
    returnVal->numOfValues = fieldToCreate->numOfValues;
    returnVal->fieldValue = (char**) malloc(sizeof(char*) * returnVal->numOfValues);
    for(int i = 0; i < returnVal->numOfValues; i++)
    {
        int len = strlen(fieldToCreate->fieldValue[i]) + 1;
        returnVal->fieldValue[i] = (char*) malloc(sizeof(char) * len);
        strcpy(returnVal->fieldValue[i], fieldToCreate->fieldValue[i]);
    }
    return returnVal;
}

char* allocateAndCopyStr(const char* oldStr)
{
    int size = strlen(oldStr) + 1;
    char* returnVal = (char *)malloc(sizeof(char) * size);
    strcpy(returnVal, oldStr);
    return returnVal;
}
