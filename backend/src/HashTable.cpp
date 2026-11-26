// HashTable.cpp — Hash table with djb2 hashing and separate chaining.

#include "HashTable.h"

using namespace std;

HashTable::HashTable(int capacity) {
    capacity_ = capacity;
    buckets_ = new LinkedList[capacity_];
}

HashTable::~HashTable() {
    delete[] buckets_;
}

// djb2 hash
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
