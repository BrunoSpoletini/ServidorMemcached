#include "hash_table.h"
#define TABLE_SIZE 1000

unsigned long hash(const char* key) {
    unsigned long hashValue = 0;
    int i = 0;
    while (key[i] != '\0') {
        hashValue = key[i] + (hashValue << 5) - hashValue;
        i++;
    }
    return hashValue % TABLE_SIZE;
}

void initHashTable(HashTable* table) {
    table->entries = (Entry*) calloc(TABLE_SIZE, sizeof(Entry));
    table->size = TABLE_SIZE;
}

void insert(HashTable* table, const char* key, int value) {
    unsigned long hashValue = hash(key);
    while (table->entries[hashValue].key != NULL) {
        if (strcmp(table->entries[hashValue].key, key) == 0) {
            table->entries[hashValue].value = value;
            return;
        }
        hashValue = (hashValue + 1) % table->size;
    }
    table->entries[hashValue].key = strdup(key);
    table->entries[hashValue].value = value;
}

int get(HashTable* table, const char* key) {
    unsigned long hashValue = hash(key);
    while (table->entries[hashValue].key != NULL) {
        if (strcmp(table->entries[hashValue].key, key) == 0) {
            return table->entries[hashValue].value;
        }
        hashValue = (hashValue + 1) % table->size;
    }
    return -1;
}

bool delete(HashTable* table, const char* key) {
    unsigned long hashValue = hash(key);
    while (table->entries[hashValue].key != NULL) {
        if (strcmp(table->entries[hashValue].key, key) == 0) {
            free(table->entries[hashValue].key);
            table->entries[hashValue].key = NULL;
            table->entries[hashValue].value = -1;
            return true;
        }
        hashValue = (hashValue + 1) % table->size;
    }
}

void printTable(HashTable* table) {
    printf("Hash Table:\n");
    for (int i = 0; i < table->size; i++) {
        if (table->entries[i].key != NULL) {
            printf("Key: %s, Value: %d\n", table->entries[i].key, table->entries[i].value);
        }
    }
}
