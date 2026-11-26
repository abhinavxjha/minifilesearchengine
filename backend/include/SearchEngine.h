/*
 * SearchEngine.h — Central orchestrator for the Mini Search Engine.
 *
 * Responsibilities:
 *   1. Coordinate document indexing across InvertedIndex and Trie.
 *   2. Tokenize and normalize search queries.
 *   3. Execute multi-term searches:
 *      - Term posting lookups via InvertedIndex
 *      - Document score aggregation
 *      - Term coordination bonus
 *      - Snippet extraction with matched term highlighting (<mark>...</mark>)
 *   4. Provide autocomplete suggestions via Trie.
 *   5. Provide engine statistics (searches executed, documents, terms).
 *
 * Note: Heap extraction and Merge Sort ranking will be integrated
 * in Phase 8 and Phase 9 respectively.
 */

#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "Document.h"
#include "InvertedIndex.h"
#include "Trie.h"
#include "Queue.h"
#include <string>
#include <vector>

using namespace std;

// ── SearchResult ──────────────────────────────────────────────
struct SearchResult {
    int docId;                              // Document ID
    string filename;                        // Document filename
    double score;                           // Calculated relevance score
    int matchedTermsCount;                  // Number of query terms matched
    vector<string> matchedWords;            // List of matched words
    string snippet;                         // Preview with highlighted keywords

    SearchResult() {
        docId = -1;
        score = 0.0;
        matchedTermsCount = 0;
    }
};

// ── SearchEngine Class ────────────────────────────────────────
class SearchEngine {
public:
    // Constructor & Destructor
    explicit SearchEngine(int tableCapacity = 503);
    ~SearchEngine();

    // ── Indexing ─────────────────────────────────────────────

    // Discovers and indexes all .txt documents in a folder using the custom Queue
    int indexDirectory(const string& directoryPath);

    // Indexes a single document into both InvertedIndex and Trie
    void addDocument(const Document& doc);

    // ── Search & Autocomplete ────────────────────────────────

    // Process a multi-word or single-word query and return scored results
    vector<SearchResult> search(const string& query);

    // Autocomplete suggestions using prefix tree
    vector<string> autocomplete(const string& prefix, int limit = 5) const;

    // ── Text Processing Helpers ──────────────────────────────

    // Tokenizes a raw query string into normalized search terms
    static vector<string> tokenizeQuery(const string& query);

    // Generates a text snippet around the first occurrence of query terms,
    // wrapping matching words in <mark>...</mark> tags
    static string generateSnippet(const string& content,
                                  const vector<string>& queryTerms,
                                  int snippetLength = 180);

    // ── Accessor ─────────────────────────────────────────────

    const InvertedIndex& getIndex() const;

private:
    InvertedIndex index_;
    Trie trie_;
    Queue indexingQueue_;
};

#endif // SEARCHENGINE_H
