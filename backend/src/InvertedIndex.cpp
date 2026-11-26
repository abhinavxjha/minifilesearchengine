/*
 * InvertedIndex.cpp — Implementation of InvertedIndex class.
 */

#include "InvertedIndex.h"

using namespace std;

InvertedIndex::InvertedIndex(int tableCapacity)
    : table_(tableCapacity) {}

InvertedIndex::~InvertedIndex() {}

void InvertedIndex::addDocument(const Document& doc) {
    documents_.push_back(doc);

    const auto& words = doc.getWords();
    for (const auto& word : words) {
        table_.insert(word, doc.getId());
    }
}

const vector<PostingEntry>* InvertedIndex::getPostings(const string& term) const {
    Node* node = table_.search(term);
    if (node != nullptr) {
        return &(node->postings);
    }
    return nullptr;
}

const Document* InvertedIndex::getDocument(int docId) const {
    if (docId >= 0 && docId < static_cast<int>(documents_.size())) {
        return &documents_[docId];
    }
    return nullptr;
}

const vector<Document>& InvertedIndex::getAllDocuments() const {
    return documents_;
}

int InvertedIndex::getTotalDocuments() const {
    return static_cast<int>(documents_.size());
}
