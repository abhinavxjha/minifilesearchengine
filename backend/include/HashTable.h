// HashTable.h — Separate-chaining hash table using djb2.

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "LinkedList.h"
#include <string>

using namespace std;

class HashTable {
public:
    explicit HashTable(int capacity = 101);
    ~HashTable();
    void insert(const string& key, int docId);
    Node* search(const string& key) const;

private:
    LinkedList* buckets_;
    int capacity_;
    int hashFunction(const string& key) const;
};

#endif
