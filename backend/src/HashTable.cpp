/*
 * HashTable.cpp — Implementation of the custom hash table.
 */

#include "HashTable.h"

using namespace std;

HashTable::HashTable(int capacity) {
    capacity_ = capacity;
    buckets_ = new LinkedList[capacity_];
}

HashTable::~HashTable() {
    delete[] buckets_;
}

/*
 * hashFunction — djb2 by Daniel J. Bernstein.
 *
 * Time:  O(k) where k = key length
 * Space: O(1)
 */
int HashTable::hashFunction(const string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + static_cast<unsigned long>(c);
    }
    return static_cast<int>(hash % static_cast<unsigned long>(capacity_));
}

void HashTable::insert(const string& key, int docId) {
    int index = hashFunction(key);
    buckets_[index].insert(key, docId);
}

Node* HashTable::search(const string& key) const {
    int index = hashFunction(key);
    return buckets_[index].search(key);
}
