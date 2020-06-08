#ifndef CITESOFT_H
#define CITESOFT_H
#include "datastruct.h"

const char* OUTPUT_FILE_NAME = "CiteSoftwareCheckPoints.txt";
const char* CONSOLIDATED_FILE_NAME = "CiteSoftwareConsolidatedLog.txt";

const char* validOptArgs[7] = {"version", "cite", "author", "doi", "url", "encoding", "misc"};
const char* reqArgs[3] = {"timestamp", "unique_id", "software_name"};

void importCite(const char* uniqueID, const char* softwareName, int argCount, ...);
char* getTimestamp();
citation_entry_t* compareSameID(citation_entry_t *oldEntry, citation_entry_t *newEntry);
void freeTable();

#endif
