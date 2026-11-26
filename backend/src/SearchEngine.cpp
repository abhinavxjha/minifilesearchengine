/*
 * SearchEngine.cpp — Implementation of the SearchEngine orchestrator.
 */

#include "SearchEngine.h"
#include "MergeSort.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <filesystem>

using namespace std;

namespace fs = filesystem;

// ════════════════════════════════════════════════════════════════
// Constructor & Destructor
// ════════════════════════════════════════════════════════════════

SearchEngine::SearchEngine(int tableCapacity) : index_(tableCapacity) {}

SearchEngine::~SearchEngine() {}

// ════════════════════════════════════════════════════════════════
// Document Indexing
// ════════════════════════════════════════════════════════════════

/*
 * indexDirectory — Scans a directory for .txt files and processes
 * them sequentially using the custom Queue.
 *
 * Algorithm:
 *   1. Discover all .txt files in the given path.
 *   2. For each file found, enqueue an IndexTask into indexingQueue_.
 *   3. While the queue is not empty:
 *      a. Dequeue task {docId, filename, filepath}.
 *      b. Load file content from disk.
 *      c. Tokenize into normalized words.
 *      d. Pass to addDocument(doc) for InvertedIndex & Trie indexing.
 *   4. Return count of successfully indexed documents.
 *
 * Time Complexity:
 *   O(F + D * W) where F = file discovery, D = documents, W = words/doc.
 */
int SearchEngine::indexDirectory(const string& directoryPath) {
    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        cerr << "[ERROR] Invalid directory: " << directoryPath << "\n";
        return 0;
    }

    // Step 1: Enqueue all document indexing tasks
    int nextId = index_.getTotalDocuments();
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            indexingQueue_.enqueue(nextId++, 
                                   entry.path().filename().string(), 
                                   entry.path().string());
        }
    }

    int processedCount = 0;

    // Step 2: Sequentially dequeue and index each document
    while (!indexingQueue_.isEmpty()) {
        IndexTask task = indexingQueue_.dequeue();

        Document doc(task.docId, task.filename);
        if (doc.loadFromFile(task.filepath)) {
            doc.tokenize();
            addDocument(doc);
            processedCount++;
        }
    }

    return processedCount;
}

/*
 * addDocument — Ingests a Document into both InvertedIndex and Trie.
 *
 * 1. Inserts into InvertedIndex (builds HashTable posting lists).
 * 2. Inserts each normalized token into Trie (builds prefix tree).
 */
void SearchEngine::addDocument(const Document& doc) {
    index_.addDocument(doc);

    for (const auto& word : doc.getWords()) {
        trie_.insert(word);
    }
}

// ════════════════════════════════════════════════════════════════
// Query Tokenization
// ════════════════════════════════════════════════════════════════

/*
 * tokenizeQuery — Breaks raw query into normalized alphanumeric tokens.
 *
 * Example: "Data-structures & Algorithms!"
 *       -> ["data", "structures", "algorithms"]
 */
vector<string> SearchEngine::tokenizeQuery(const string& query) {
    vector<string> tokens;
    string current;

    for (char ch : query) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
            current += ch;
        } else if (ch >= 'A' && ch <= 'Z') {
            current += static_cast<char>(ch + ('a' - 'A'));
        } else {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

// ════════════════════════════════════════════════════════════════
// Snippet Generation & Term Highlighting
// ════════════════════════════════════════════════════════════════

/*
 * generateSnippet — Produces a readable preview highlighting query terms.
 *
 * Algorithm:
 *   1. Search for the first case-insensitive occurrence of any query word.
 *   2. Center a window of ~snippetLength characters around that match.
 *   3. Wrap all query term occurrences within the window in <mark>...</mark>.
 */
string SearchEngine::generateSnippet(const string& content, 
                                          const vector<string>& queryTerms, 
                                          int snippetLength) {
    if (content.empty()) return "";
    if (static_cast<int>(content.size()) <= snippetLength) {
        return content;
    }

    // Lowercase version of content for matching positions
    string lowerContent = content;
    for (char& c : lowerContent) {
        if (c >= 'A' && c <= 'Z') c = c + ('a' - 'A');
    }

    // Find first occurrence of any query term
    size_t firstPos = string::npos;
    for (const auto& term : queryTerms) {
        if (term.empty()) continue;
        size_t pos = lowerContent.find(term);
        if (pos != string::npos && (firstPos == string::npos || pos < firstPos)) {
            firstPos = pos;
        }
    }

    // Determine window boundaries
    size_t start = 0;
    if (firstPos != string::npos && firstPos > 30) {
        start = firstPos - 30; // 30 characters of context before match
    }

    // Find preceding space to avoid cutting a word in half
    if (start > 0) {
        size_t spacePos = content.find(' ', start);
        if (spacePos != string::npos && spacePos < start + 20) {
            start = spacePos + 1;
        }
    }

    size_t end = start + snippetLength;
    if (end > content.size()) {
        end = content.size();
    } else {
        size_t spacePos = content.rfind(' ', end);
        if (spacePos != string::npos && spacePos > start) {
            end = spacePos;
        }
    }

    string window = content.substr(start, end - start);
    string prefixDots = (start > 0) ? "... " : "";
    string suffixDots = (end < content.size()) ? " ..." : "";

    // Highlight matching query words with <mark> tags
    string highlighted;
    size_t i = 0;
    while (i < window.size()) {
        bool matched = false;
        for (const auto& term : queryTerms) {
            if (term.empty()) continue;
            size_t tLen = term.size();
            if (i + tLen <= window.size()) {
                // Case-insensitive comparison
                bool matchWord = true;
                for (size_t k = 0; k < tLen; k++) {
                    char c1 = window[i + k];
                    if (c1 >= 'A' && c1 <= 'Z') c1 = c1 + ('a' - 'A');
                    if (c1 != term[k]) {
                        matchWord = false;
                        break;
                    }
                }
                if (matchWord) {
                    highlighted += "<mark>";
                    highlighted += window.substr(i, tLen);
                    highlighted += "</mark>";
                    i += tLen;
                    matched = true;
                    break;
                }
            }
        }
        if (!matched) {
            highlighted += window[i];
            i++;
        }
    }

    return prefixDots + highlighted + suffixDots;
}

// ════════════════════════════════════════════════════════════════
// Search & Relevance Calculation
// ════════════════════════════════════════════════════════════════

/*
 * search — Executes a full-text search across all indexed documents.
 *
 * Algorithm:
 *   1. Tokenize the raw search query into normalized terms.
 *   2. For each query term:
 *      - Retrieve posting list from InvertedIndex via Hash Table lookup.
 *      - For each document posting {docId, frequency}:
 *        accumulate Term Frequency (TF) and track matched terms.
 *   3. Compute Relevance Score:
 *      score(d, Q) = sum(tf(term, d)) + (matchedTermsCount * 2.0)
 *      - The first component rewards frequency (how often terms appear).
 *      - The second component provides a Coordination Bonus (rewarding
 *        documents that match multiple distinct query terms).
 *   4. Construct SearchResult objects with snippets and metadata.
 *
 * Time Complexity:
 *   O(T * P) where T = number of query terms, P = average posting list length.
 *   For typical queries (1-4 words), this executes in under a millisecond!
 */
vector<SearchResult> SearchEngine::search(const string& query) {
    vector<SearchResult> results;
    vector<string> queryTerms = tokenizeQuery(query);
    if (queryTerms.empty()) {
        return results;
    }

    int totalDocs = index_.getTotalDocuments();
    if (totalDocs == 0) return results;

    // Accumulators for matching documents
    struct DocScoreAccumulator {
        int totalTf = 0;
        int matchedTermsCount = 0;
        vector<string> matchedWords;
    };

    vector<DocScoreAccumulator> docScores(totalDocs);
    vector<bool> hasMatched(totalDocs, false);

    // Step 2: Accumulate postings for each term
    for (const auto& term : queryTerms) {
        const auto* postings = index_.getPostings(term);
        if (postings == nullptr) continue;

        for (const auto& entry : *postings) {
            int docId = entry.docId;
            if (docId >= 0 && docId < totalDocs) {
                docScores[docId].totalTf += entry.frequency;
                docScores[docId].matchedTermsCount++;
                docScores[docId].matchedWords.push_back(term);
                hasMatched[docId] = true;
            }
        }
    }

    // Step 3: Compute final scores & assemble results
    for (int docId = 0; docId < totalDocs; docId++) {
        if (!hasMatched[docId]) continue;

        const Document* doc = index_.getDocument(docId);
        if (doc == nullptr) continue;

        SearchResult res;
        res.docId = docId;
        res.filename = doc->getFilename();
        res.matchedTermsCount = docScores[docId].matchedTermsCount;
        res.matchedWords = docScores[docId].matchedWords;

        // Relevance Formula:
        // Base = Sum of TF
        // Bonus = Coordination Match (matching multiple terms of query)
        res.score = docScores[docId].totalTf + (res.matchedTermsCount * 2.0);

        // Generate preview snippet with matched terms highlighted
        res.snippet = generateSnippet(doc->getContent(), queryTerms, 180);

        results.push_back(res);
    }

    // Step 4: Rank search results in descending order using recursive MergeSort
    MergeSort::sort(results);

    return results;
}

/*
 * autocomplete — Fetches suggestions from Trie.
 */
vector<string> SearchEngine::autocomplete(const string& prefix, int limit) const {
    return trie_.getSuggestions(prefix, limit);
}

// ════════════════════════════════════════════════════════════════
// Accessor
// ════════════════════════════════════════════════════════════════

const InvertedIndex& SearchEngine::getIndex() const {
    return index_;
}
