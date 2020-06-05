#ifndef DATASTRUCT_H
#define DATASTRUCT_H

typedef struct const_field_t
{
    struct const_field_t* nextField;
    const char* fieldName;
    int numOfValues;
    const char** fieldValue;
} const_field_t;

typedef struct field_t
{
    struct field_t* nextField;
    char* fieldName;
    int numOfValues;
    char** fieldValue;
} field_t;

typedef struct citation_entry_t
{
    char* uniqueID;
    char* softwareName;
    char* timestamp;
    char* version;//This is simply a pointer to a field_t in fields with fieldName == "version" iff it exists, otherwise it is null
    struct field_t* fields;
} citation_entry_t;


void destroyCitation(citation_entry_t* entry);
void destroyFieldList(field_t* firstEntry);
field_t* copyField(field_t* fieldToCopy);
field_t* copyFieldList(field_t* firstEntry);
field_t* createField(const_field_t* fieldToCreate);
field_t* createFieldList(const_field_t* firstEntry);
char* allocateAndCopyStr(const char* oldStr);

#endif
