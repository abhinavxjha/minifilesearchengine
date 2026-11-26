// LinkedList.h — Singly linked list for hash table collision chains.

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <string>
#include <vector>

using namespace std;

struct PostingEntry {
    int docId;
    int frequency;

    PostingEntry(int id, int freq) {
        docId = id;
        frequency = freq;
    }
};

struct Node {
    string key;
    vector<PostingEntry> postings;
    Node* next;

    Node(const string& k) {
        key = k;
        next = nullptr;
    }
};

class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    void insert(const string& key, int docId);
    Node* search(const string& key) const;

private:
    Node* head_;
};

#endif
