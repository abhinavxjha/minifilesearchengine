/*
 * Trie.cpp — Implementation of the custom Trie data structure.
 */

#include "Trie.h"

using namespace std;

// ════════════════════════════════════════════════════════════════
// TrieNode Implementation
// ════════════════════════════════════════════════════════════════

TrieNode::TrieNode() {
    isEndOfWord = false;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        children[i] = nullptr;
    }
}

TrieNode::~TrieNode() {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (children[i] != nullptr) {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

// ════════════════════════════════════════════════════════════════
// Trie Implementation
// ════════════════════════════════════════════════════════════════

Trie::Trie() {
    root_ = new TrieNode();
}

Trie::~Trie() {
    delete root_;
}

void Trie::insert(const string& word) {
    if (word.empty()) return;

    TrieNode* current = root_;

    for (char ch : word) {
        unsigned char index = static_cast<unsigned char>(ch);
        if (index >= TrieNode::ALPHABET_SIZE) continue;

        if (current->children[index] == nullptr) {
            current->children[index] = new TrieNode();
        }
        current = current->children[index];
    }

    current->isEndOfWord = true;
}

vector<string> Trie::getSuggestions(const string& prefix, int limit) const {
    vector<string> suggestions;
    if (prefix.empty() || limit <= 0) return suggestions;

    TrieNode* current = root_;

    for (char ch : prefix) {
        unsigned char index = static_cast<unsigned char>(ch);
        if (index >= TrieNode::ALPHABET_SIZE) return suggestions;

        if (current->children[index] == nullptr) {
            return suggestions; // Prefix does not exist
        }
        current = current->children[index];
    }

    if (current->isEndOfWord) {
        suggestions.push_back(prefix);
    }

    string currentWord = prefix;
    collectSuggestions(current, currentWord, suggestions, limit);

    return suggestions;
}

void Trie::collectSuggestions(TrieNode* current,
                              string& currentWord,
                              vector<string>& suggestions,
                              int limit) const {
    if (current == nullptr || static_cast<int>(suggestions.size()) >= limit) {
        return;
    }

    for (int i = 0; i < TrieNode::ALPHABET_SIZE; i++) {
        if (current->children[i] != nullptr) {
            char ch = static_cast<char>(i);
            currentWord.push_back(ch);

            if (current->children[i]->isEndOfWord) {
                if (suggestions.empty() || suggestions.back() != currentWord) {
                    suggestions.push_back(currentWord);
                    if (static_cast<int>(suggestions.size()) >= limit) {
                        currentWord.pop_back();
                        return;
                    }
                }
            }

            collectSuggestions(current->children[i], currentWord, suggestions, limit);
            currentWord.pop_back(); // Backtrack
        }
    }
}
