/*
 * LinkedList.h — Custom singly linked list for hash table chaining.
 *
 * Each node stores:
 *   - key:      A word (string)
 *   - postings: List of {docId, frequency} pairs indicating which
 *               documents contain this word and how often
 *   - next:     Pointer to the next node in the chain
 *
 * Complexity:
 *   - Insert (at head):  O(1)
 *   - Insert (with key lookup): O(n) where n = chain length
 *   - Search:  O(n)
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <string>
#include <vector>

using namespace std;

struct PostingEntry {
    int docId;       // Which document
    int frequency;   // How many times the word appears in that document

    PostingEntry(int id, int freq) {
        docId = id;
        frequency = freq;
    }
};

struct Node {
    string key;                      // The word
    vector<PostingEntry> postings;   // Which documents contain it
    Node* next;                      // Pointer to next node in chain

    Node(const string& k) {
        key = k;
        next = nullptr;
    }
};

// Singly linked list used as a collision chain in the hash table.
class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    // Insert a word with a document ID (or update frequency if present).
    void insert(const string& key, int docId);

    // Search for a word in the chain.
    Node* search(const string& key) const;

private:
    Node* head_;    // Pointer to the first node
};

#endif 
