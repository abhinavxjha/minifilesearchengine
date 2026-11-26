// SearchEngine.h — Core indexing, search, and autocomplete orchestrator.

#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "Document.h"
#include "InvertedIndex.h"
#include "Trie.h"
#include "Queue.h"
#include <string>
#include <vector>

using namespace std;

struct SearchResult {
    int docId;
    string filename;
    double score;
    int matchedTermsCount;
    vector<string> matchedWords;
    string snippet;

    SearchResult() {
        docId = -1;
        score = 0.0;
        matchedTermsCount = 0;
    }
};

class SearchEngine {
public:
    explicit SearchEngine(int tableCapacity = 503);
    ~SearchEngine();

    int indexDirectory(const string& directoryPath);
    void addDocument(const Document& doc);

    vector<SearchResult> search(const string& query);
    vector<string> autocomplete(const string& prefix, int limit = 5) const;

    static vector<string> tokenizeQuery(const string& query);
    static string generateSnippet(const string& content,
                                  const vector<string>& queryTerms,
                                  int snippetLength = 180);

    const InvertedIndex& getIndex() const;

private:
    InvertedIndex index_;
    Trie trie_;
    Queue indexingQueue_;
};

#endif // SEARCHENGINE_H
