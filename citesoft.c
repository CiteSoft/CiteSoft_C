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
#include <stdarg.h>
#include <string.h>
#include <time.h>
//Not needed until consolidate is implemented #include <yaml.h>
#include <regex.h>
#include "semver.h"
#include "citesoft.h"
#include "datastruct.h"
#include "stringhash.h"


string_hash_table_t hashTable;

//void importCite(const char* uniqueID, const char* softwareName, field_t *fields)
//{
//    addCitation(uniqueID, softwareName, fields);
//}



//fields must be preallocated!!!
void addCitation(const char* uniqueID, const char* softwareName, field_t *fields)
{
    citation_entry_t *newEntry = malloc(sizeof(citation_entry_t));
    newEntry->uniqueID = allocateAndCopyStr(uniqueID);
    newEntry->softwareName = allocateAndCopyStr(softwareName);
    newEntry->fields = fields;
    field_t* fieldPtr = newEntry->fields;
    while(fieldPtr)
    {
        if(!strcmp(fieldPtr->fieldName, "version"))
        {
            newEntry->version = fieldPtr->fieldValue[0];
            break;
        }
        else
        {
            fieldPtr = fieldPtr->nextField;
        }
    }
    newEntry->timestamp = getTimestamp();
    addItem(&hashTable, newEntry, &compareSameID);
}

void addConstCitation(const char* uniqueID, const char* softwareName, const_field_t *fields)
{
    addCitation(uniqueID, softwareName, createFieldList(fields));
}

void addVarCitation(const char* uniqueID, const char* softwareName, field_t *fields)
{
    addCitation(uniqueID, softwareName, copyFieldList(fields));
}

void importCite(const char* uniqueID, const char* softwareName, int argCount, ...)
{
    if(argCount == 0)
    {
        addCitation(uniqueID, softwareName, NULL);
    }
    else
    {
        const_field_t* fields = malloc(sizeof(const_field_t) * argCount);
        va_list valist;
        va_start(valist, argCount); //initialize valist for argcount number of arguments
        for (int i = 0; i < argCount; i++)
        {
            //access all the arguments assigned to valist
            fields[i] = va_arg(valist, const_field_t);
        }
        va_end(valist); //clean memory reserved for valist
        for (int i = 0; i < argCount - 1; i++)//Loop through all but last arg
        {
            fields[i].nextField = &fields[i + 1];
        }
        fields[argCount - 1].nextField = NULL;
        addConstCitation(uniqueID, softwareName, fields);
        free(fields);
    }
}

void appendCitationToFile(FILE *fPtr, citation_entry_t *entry)
{
    fputs("-\r\n", fPtr);
    for(int i = 0; i < sizeof(reqArgs)/sizeof(reqArgs[0]); i++)
    {
        fputs("    ", fPtr);
        fputs(reqArgs[i], fPtr);
        fputs(": >-\r\n", fPtr);
        fputs("        ", fPtr);
        switch(i)
        {
            case 0:
                fputs(entry->timestamp, fPtr);
                break;
            case 1:
                fputs(entry->uniqueID, fPtr);
                break;
            case 2:
                fputs(entry->softwareName, fPtr);
                break;
            default:
                fputs("An error occurred", fPtr);
                break;
        }
        fputs("\r\n", fPtr);
    }
    field_t *fieldPtr = entry->fields;
    while(fieldPtr)
    {
        fputs("    ", fPtr);
        fputs(fieldPtr->fieldName, fPtr);
        fputs("\r\n", fPtr);
        for(int i = 0; i < fieldPtr->numOfValues; i++)
        {
            fputs("        - >-\r\n", fPtr);
            fputs("            ", fPtr);
            fputs(fieldPtr->fieldValue[i], fPtr);
            fputs("\r\n", fPtr);
        }
        fieldPtr = fieldPtr->nextField;
    }
}


void compileCiteSoftwareLog()
{
    item_list_t* list = getAllItems(&hashTable);
    item_list_t* ptr = list;

    FILE *file = fopen(OUTPUT_FILE_NAME, "a");
    fputs("---\r\n", file);
    while(ptr)
    {
        appendCitationToFile(file, ptr->value);
        ptr = ptr->nextItem;
    }
    destroyList(list);
}

void compileCiteSoftwareLogAndDestroy()
{
    compileCiteSoftwareLog();
    clean();
}

void consolidateSoftwareLog()
{
    //TODO
}

char* getTimestamp()
{
    char result[100];
    time_t t = time(NULL);
    struct tm *timeInfo = localtime(&t);
    size_t actualSize = strftime(result, sizeof(result), "%Y-%m-%dT%H:%M:%S", timeInfo);
    char* returnVal = malloc(actualSize);
    strcpy(returnVal, result);
}

void clean()
{
    destroyTable(&hashTable, &destroyCitation);
}

citation_entry_t* compareSameID(citation_entry_t *oldEntry, citation_entry_t *newEntry)
{
    if(oldEntry->version && newEntry->version)
    {
      semver_t old_version = {};
      semver_t new_version = {};

      if (semver_parse(oldEntry->version, &old_version) || semver_parse(newEntry->version, &new_version))
      {
          //Error while parsing semantic version
          regex_t regex;
          regcomp(&regex, "[0-9]*\.[0-9]*", 0);
          if(!regexec(&regex, oldEntry->version, 0, NULL, 0) ||
             !regexec(&regex, newEntry->version, 0, NULL, 0))
          {//At least one version string did not match the regex, alphanumric comp
              if(strcmp(oldEntry->version, newEntry->version) >= 0)
              {
                  destroyCitation(newEntry);
                  return oldEntry;
              }
              else
              {
                  destroyCitation(oldEntry);
                  return newEntry;
              }
          }
          else
          {//Parse both version strings as a double
              double oldVersionNum = atof(oldEntry->version);
              double newVersionNum = atof(oldEntry->version);
              if(oldVersionNum >= newVersionNum)
              {
                  destroyCitation(newEntry);
                  return oldEntry;
              }
              else
              {
                  destroyCitation(oldEntry);
                  return newEntry;
              }
          }
      }
      int result = semver_compare(old_version, new_version);
      if (result == 0)
      {
          //Versions are equivalent, save newer entry
          destroyCitation(newEntry);
          return oldEntry;
      }
      else if (result == -1)
      {
          //Old version is lower than new version
          destroyCitation(oldEntry);
          return newEntry;
      }
      else
      {
          //Old version is higher than new version
          destroyCitation(newEntry);
          return oldEntry;
      }

      // Free allocated memory when we're done
      semver_free(&old_version);
      semver_free(&new_version);
    }
    else if(oldEntry->version)//Old entry has a version and new entry doesn't
    {
        destroyCitation(newEntry);
        return oldEntry;
    }
    else if(newEntry->version)//New entry has a version and old entry doesn't
    {
        destroyCitation(oldEntry);
        return newEntry;
    }
    else//Neither entry has a version, keep old entry
    {
        destroyCitation(newEntry);
        return oldEntry;
    }


    return NULL;
}

void testOpFields()
{
    printf("Testing optional fields...\r\n");
    //Optional field 1
    const_field_t f1;
    f1.fieldName = "cite";
    const char* values[] = {"DOI:example", "This is another citation"};
    f1.fieldValue = values;
    f1.numOfValues = sizeof(values)/sizeof(char*);
    //Optional field 2
    const_field_t f2;
    f2.fieldName = "author";
    const char* values2[] = {"CPH"};
    f2.fieldValue = values2;
    f2.numOfValues = sizeof(values2)/sizeof(char*);
    importCite("Optional fields", "CiteSoft", 2, f1, f2);
}

int main(int argc, char* argv[])
{
    printf("Adding ID1 to log\r\n");
    importCite("ID1", "CiteSoft", 0);
    printf("Adding ID2 to log\r\n");
    importCite("ID2", "CiteSoft", 0);
    testOpFields();
    printf("Exporting log...\r\n");
    compileCiteSoftwareLogAndDestroy();
    return 0;
}
