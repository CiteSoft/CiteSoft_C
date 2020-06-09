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

//Field list(fields) must be preallocated - this function will only copy pointers to the field list
//Add citation entry to the hash table
void addCitation(const char* uniqueID, const char* softwareName, field_t *fields)
{
    //Allocate memory for the citation and populate constant fields
    citation_entry_t *newEntry = malloc(sizeof(citation_entry_t));
    newEntry->uniqueID = allocateAndCopyStr(uniqueID);
    newEntry->softwareName = allocateAndCopyStr(softwareName);
    newEntry->timestamp = getTimestamp();
    newEntry->fields = fields;
    field_t* fieldPtr = newEntry->fields;
    newEntry->version = NULL;//Ensure that version pointer is NULL if no version is specified
    //Check optional field for "version", if present, add a pointer to the first value in the last(to use later for citation comparisons)
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
    addItem(&hashTable, newEntry, &compareSameID);//Add item to hash table
}

//Add a citation with a linked list of fields containing unmutable field strings
void addConstCitation(const char* uniqueID, const char* softwareName, const_field_t *fields)
{
    addCitation(uniqueID, softwareName, createFieldList(fields));
}

//Add a citation with a linked list of fields containing mutable field strings
void addVarCitation(const char* uniqueID, const char* softwareName, field_t *fields)
{
    addCitation(uniqueID, softwareName, copyFieldList(fields));
}

//Manually add a citation with an arbitrary number of fields to the hash table
void importCite(const char* uniqueID, const char* softwareName, int argCount, ...)
{
    if(argCount == 0)
    {
        //If no optional fields are specified, proceed directly with adding
        addCitation(uniqueID, softwareName, NULL);
    }
    else
    {
        //Otherwise, load optional fields into a linked list
        const_field_t* citationFields = malloc(sizeof(const_field_t) * argCount);
        va_list varArgList;
        va_start(varArgList, argCount); //initialize varArgList for argCount number of arguments
        for (int i = 0; i < argCount; i++)
        {
            //access all the arguments assigned to varArgList
            citationFields[i] = va_arg(varArgList, const_field_t);
        }
        va_end(varArgList); //clean memory reserved for varArgList
        for (int i = 0; i < argCount - 1; i++)//Loop through all but last arg(last arg "next" pointer doesn't need to be updated)
        {
            citationFields[i].nextField = &citationFields[i + 1];
        }
        citationFields[argCount - 1].nextField = NULL;//Last pointer should be null to terminate the list
        addConstCitation(uniqueID, softwareName, citationFields);
        free(citationFields);//Free the array used to store the fields temporarily(addConstCitation will copy them)
    }
}

//Write the YAML for a single entry to the specified file pointer
//File pointer must allow writes
void appendCitationToFile(FILE *fPtr, citation_entry_t *entry)
{
    //Print entry header
    fputs("-\r\n", fPtr);
    for(int i = 0; i < sizeof(reqArgs)/sizeof(reqArgs[0]); i++)
    {
        fputs("    ", fPtr);
        //Print required argument label
        fputs(reqArgs[i], fPtr);
        //Print YAML syntax for required arguments from standard
        fputs(": >-\r\n", fPtr);
        fputs("        ", fPtr);
        //Print the correct data field for the label stored in reqArgs[i]
        //This array can be seen in citesoft.h
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
    //Loop through all fields
    while(fieldPtr)
    {
        //Print the name of the field with proper indentation
        fputs("    ", fPtr);
        fputs(fieldPtr->fieldName, fPtr);
        fputs("\r\n", fPtr);
        //For every item in list of the current optional field at fieldPtr
        for(int i = 0; i < fieldPtr->numOfValues; i++)
        {
            //Print a new line for each item in the list
            //Print YAML syntax from standard for item of optional field
            fputs("        - >-\r\n", fPtr);
            fputs("            ", fPtr);
            //Print value of the line
            fputs(fieldPtr->fieldValue[i], fPtr);
            fputs("\r\n", fPtr);
        }
        //Advance to next field(or break loop at end of list with NULL pointer)
        fieldPtr = fieldPtr->nextField;
    }
}

//Append every citation to the log file at the specified path
void compileCiteSoftwareLog(const char* path)
{
    //Get all items in the hash table as a linked list
    item_list_t* citationList = getAllItems(&hashTable);
    //Create pointer for iterating through the list
    item_list_t* itemPtr = citationList;
    int concatLen = strlen(OUTPUT_FILE_NAME) + 1;
    if(path)
    {
        //If path is used, make space for path in concat string
        concatLen += strlen(path);
    }
    char* pathWithFilename = malloc(concatLen * sizeof(char));
    if(path)
    {
        //If the user specified a path, concatencate it with OUTPUT_FILE_NAME
        strcpy(pathWithFilename, OUTPUT_FILE_NAME);
        strcat(pathWithFilename, path);
    }
    else
    {
        //Otherwise, saving to current dir, can just pass file name
        strcpy(pathWithFilename, OUTPUT_FILE_NAME);
    }

    //Open the output file to append
    FILE *file = fopen(pathWithFilename, "a");
    //Free path string, not needed any more
    free(pathWithFilename);
    //Append a YAML file header
    fputs("---\r\n", file);
    //For every citation in the list
    while(itemPtr)
    {
        //Append the citation to the file
        appendCitationToFile(file, itemPtr->value);
        //Advance to next item in the list or, if ptr is the last item in the list, break the loop(nextItem == NULL)
        itemPtr = itemPtr->nextItem;
    }
    //Free the file pointer
    fclose(file);
    //Free the memory used by the list returned by the hash table
    destroyList(citationList);
}

//Append every citation to a log file in the current directory
void compileLocalCiteSoftwareLog()
{
    compileCiteSoftwareLog(NULL);
}

//Export a CiteSoft log containing all entries currently in the hash table to specified path and free all associated memory
void compileCiteSoftwareLogAndFree(const char* path)
{
    compileCiteSoftwareLog(path);
    freeTable();
}

//Export a local CiteSoft log containing all entries currently in the hash table and free all associated memory
void compileLocalCiteSoftwareLogAndFree()
{
    compileCiteSoftwareLog(NULL);
    freeTable();
}


void consolidateSoftwareLog()
{
    //TODO
}

//Returns a pointer to a string containing the current time in ISO 8601 format
//The caller is responsible for freeing this memory
char* getTimestamp()
{
    char result[100];
    time_t t = time(NULL);
    struct tm *timeInfo = localtime(&t);
    size_t actualSize = strftime(result, sizeof(result), "%Y-%m-%dT%H:%M:%S", timeInfo) + 1;
    char* returnVal = malloc(sizeof(char) * actualSize);


    strcpy(returnVal, result);
    return returnVal;
}

//Free all memory used by the hash table, including all memory allocated for items(citations)
void freeTable()
{
    destroyTable(&hashTable, &destroyCitation);
}

//Compares two citation entries with the same uniqueID
//Returns a pointer to the newer citation(citation to keep) and destroys(frees) the older citation
citation_entry_t* compareSameID(citation_entry_t *oldEntry, citation_entry_t *newEntry)
{
    if(oldEntry->version && newEntry->version)
    {
      semver_t old_version = {};
      semver_t new_version = {};
      int parseOldFailure = semver_parse(oldEntry->version, &old_version);
      int parseNewFailure = semver_parse(newEntry->version, &new_version);
      if (parseOldFailure && parseNewFailure)
      {
          //Error while parsing semantic version of both entries
          regex_t decimalRegex;
          regcomp(&decimalRegex, "[0-9]*\\.[0-9]*", 0);
          if(regexec(&decimalRegex, oldEntry->version, 0, NULL, 0) ||
             regexec(&decimalRegex, newEntry->version, 0, NULL, 0))
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
      else if(parseOldFailure)
      {
          //Failure parsing old version, success parsing new version - return new version
          destroyCitation(oldEntry);
          return newEntry;
      }
      else if(parseNewFailure)
      {
        //Failure parsing new version, success parsing old version - return old version
          destroyCitation(newEntry);
          return oldEntry;
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
    else if(oldEntry->version)//Old entry has a version and new entry doesn't - retain old entry
    {
        destroyCitation(newEntry);
        return oldEntry;
    }
    else if(newEntry->version)//New entry has a version and old entry doesn't - retain new entry
    {
        destroyCitation(oldEntry);
        return newEntry;
    }
    else//Neither entry has a version, keep old entry
    {
        destroyCitation(newEntry);
        return oldEntry;
    }
}

//Run test case with optional fields
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

void testVersion()
{
    printf("Testing version comparison...\r\n");
  //Version 1
    const_field_t ver1;
    ver1.fieldName = "version";
    const char* values[] = {"3.2.5"};
    ver1.fieldValue = values;
    ver1.numOfValues = sizeof(values)/sizeof(char*);
    importCite("Version test", "CiteSoft", 1, ver1);
    //Version 2
    const_field_t ver2;
    ver2.fieldName = "version";
    const char* values2[] = {"8.52"};
    ver2.fieldValue = values2;
    ver2.numOfValues = sizeof(values2)/sizeof(char*);
    importCite("Version test", "CiteSoft", 1, ver2);
    //Version 3
    const_field_t ver3;
    ver3.fieldName = "version";
    const char* values3[] = {"1.20"};
    ver3.fieldValue = values3;
    ver3.numOfValues = sizeof(values3)/sizeof(char*);
    importCite("Version test", "CiteSoft", 1, ver3);
}


int main(int argc, char* argv[])
{
    printf("Adding ID1 to log\r\n");
    importCite("ID1", "CiteSoft", 0);
    printf("Adding ID2 to log\r\n");
    importCite("ID2", "CiteSoft", 0);
    testOpFields();
    testVersion();
    printf("Exporting log...\r\n");
    compileLocalCiteSoftwareLogAndFree();
    return 0;
}
