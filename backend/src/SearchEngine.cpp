// SearchEngine.cpp — Document indexing, query search, snippet highlighting, and autocomplete.

#include "SearchEngine.h"
#include "MergeSort.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <filesystem>

using namespace std;

namespace fs = filesystem;

SearchEngine::SearchEngine(int tableCapacity) : index_(tableCapacity) {}

SearchEngine::~SearchEngine() {}

int SearchEngine::indexDirectory(const string& directoryPath) {
    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
        cerr << "[ERROR] Invalid directory: " << directoryPath << "\n";
        return 0;
    }

    int nextId = index_.getTotalDocuments();
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            indexingQueue_.enqueue(nextId++, 
                                   entry.path().filename().string(), 
                                   entry.path().string());
        }
    }

    int processedCount = 0;

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

void SearchEngine::addDocument(const Document& doc) {
    index_.addDocument(doc);

    for (const auto& word : doc.getWords()) {
        trie_.insert(word);
    }
}

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

string SearchEngine::generateSnippet(const string& content, 
                                          const vector<string>& queryTerms, 
                                          int snippetLength) {
    if (content.empty()) return "";
    if (static_cast<int>(content.size()) <= snippetLength) {
        return content;
    }

    string lowerContent = content;
    for (char& c : lowerContent) {
        if (c >= 'A' && c <= 'Z') c = c + ('a' - 'A');
    }

    size_t firstPos = string::npos;
    for (const auto& term : queryTerms) {
        if (term.empty()) continue;
        size_t pos = lowerContent.find(term);
        if (pos != string::npos && (firstPos == string::npos || pos < firstPos)) {
            firstPos = pos;
        }
    }

    // Keep 30 chars of leading context and align boundaries to spaces.
    size_t start = 0;
    if (firstPos != string::npos && firstPos > 30) {
        start = firstPos - 30;
    }

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

    string highlighted;
    size_t i = 0;
    while (i < window.size()) {
        bool matched = false;
        for (const auto& term : queryTerms) {
            if (term.empty()) continue;
            size_t tLen = term.size();
            if (i + tLen <= window.size()) {
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

vector<SearchResult> SearchEngine::search(const string& query) {
    vector<SearchResult> results;
    vector<string> queryTerms = tokenizeQuery(query);
    if (queryTerms.empty()) {
        return results;
    }

    int totalDocs = index_.getTotalDocuments();
    if (totalDocs == 0) return results;

    struct DocScoreAccumulator {
        int totalTf = 0;
        int matchedTermsCount = 0;
        vector<string> matchedWords;
    };

    vector<DocScoreAccumulator> docScores(totalDocs);
    vector<bool> hasMatched(totalDocs, false);

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

    for (int docId = 0; docId < totalDocs; docId++) {
        if (!hasMatched[docId]) continue;

        const Document* doc = index_.getDocument(docId);
        if (doc == nullptr) continue;

        SearchResult res;
        res.docId = docId;
        res.filename = doc->getFilename();
        res.matchedTermsCount = docScores[docId].matchedTermsCount;
        res.matchedWords = docScores[docId].matchedWords;

        // Relevance = sum(TF) + coordination bonus per matched term
        res.score = docScores[docId].totalTf + (res.matchedTermsCount * 2.0);
        res.snippet = generateSnippet(doc->getContent(), queryTerms, 180);

        results.push_back(res);
    }

    MergeSort::sort(results);

    return results;
}

vector<string> SearchEngine::autocomplete(const string& prefix, int limit) const {
    return trie_.getSuggestions(prefix, limit);
}

const InvertedIndex& SearchEngine::getIndex() const {
    return index_;
}
