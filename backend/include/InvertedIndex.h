/*
 * InvertedIndex.h — Maps words to the documents containing them.
 *
 * Complexity:
 *   - Add Document:  O(W) where W is the number of words in the document
 *   - Term Lookup:   O(1) average time via custom HashTable
 */

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
    HashTable table_;             // Custom Hash Table storing postings
    vector<Document> documents_;  // Registry of indexed documents
};

#endif // INVERTEDINDEX_H
