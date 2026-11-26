#include "LinkedList.h"
using namespace std;

LinkedList::LinkedList() {
    head_ = nullptr;
}

LinkedList::~LinkedList() {
    Node* current = head_;
    while (current != nullptr) {
        Node* toDelete = current;
        current = current->next;
        delete toDelete;
    }
    head_ = nullptr;
}

/*
 * insert — Adds a word-document pair to the chain.
 *
 * Three cases:
 *   1. Word exists AND docId already in postings → increment frequency
 *   2. Word exists BUT docId is new → add new PostingEntry
 *   3. Word doesn't exist → create new node at HEAD
 */
void LinkedList::insert(const string& key, int docId) {
    Node* existing = search(key);

    if (existing != nullptr) {
        for (auto& posting : existing->postings) {
            if (posting.docId == docId) {
                posting.frequency++;
                return;
            }
        }
        existing->postings.push_back(PostingEntry(docId, 1));
        return;
    }

    Node* newNode = new Node(key);
    newNode->postings.push_back(PostingEntry(docId, 1));
    newNode->next = head_;
    head_ = newNode;
}

/*
 * search — Finds a word in the chain.
 *
 * Time: O(n) where n = chain length
 */
Node* LinkedList::search(const string& key) const {
    Node* current = head_;

    while (current != nullptr) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }

    return nullptr;
}
