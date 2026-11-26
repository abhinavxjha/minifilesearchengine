/*
 * HashTable.h — Custom hash table with separate chaining.
 *
 * Hash function: djb2 (Daniel J. Bernstein)
 *   hash = 5381
 *   for each char c: hash = hash * 33 + c
 *   return hash % capacity
 *
 * Complexity (average case with good hash function):
 *   - Insert: O(1)
 *   - Search: O(1)
 */

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
    LinkedList* buckets_;      // Array of linked list chains
    int capacity_;             // Number of buckets
    int hashFunction(const string& key) const;
};

#endif
