#ifndef _HASH_TABLE
#define _HASH_TABLE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char* key;
    int value;
} Entry;

typedef struct {
    Entry* entries;
    int size;
} HashTable;
unsigned long hash(const char* key);
void initHashTable(HashTable* table);
void insert(HashTable* table, const char* key, int value);
int get(HashTable* table, const char* key);
bool delete(HashTable* table, const char* key);
void printTable(HashTable* table);

#include "hash_table.c"
#endif
