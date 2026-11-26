// InvertedIndex.h — Maps terms to document posting lists via HashTable.

#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include "HashTable.h"
#include "Document.h"
#include <string>
#include <vector>

using namespace std;

class InvertedIndex {
public:
    explicit InvertedIndex(int tableCapacity = 503);
    ~InvertedIndex();

    void addDocument(const Document& doc);

    const vector<PostingEntry>* getPostings(const string& term) const;
    const Document* getDocument(int docId) const;
    const vector<Document>& getAllDocuments() const;
    int getTotalDocuments() const;

private:
    HashTable table_;
    vector<Document> documents_;
};

#endif // INVERTEDINDEX_H
