/*
 * Trie.h — Custom Trie (Prefix Tree) for Autocomplete.
 *
 * Complexity:
 *   - Insert: O(L) where L is length of the word
 *   - Autocomplete Suggestions: O(P + K) where P is prefix length, K is nodes in subtree
 */

#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

using namespace std;

// ── Trie Node ─────────────────────────────────────────────────
struct TrieNode {
    static const int ALPHABET_SIZE = 128;
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;

    TrieNode();
    ~TrieNode();    // Recursively deletes child nodes
};

// ── Trie Class ────────────────────────────────────────────────
class Trie {
public:
    Trie();
    ~Trie();

    // Insert a word into the Trie.
    void insert(const string& word);

    // Autocomplete: Returns up to 'limit' words starting with prefix.
    vector<string> getSuggestions(const string& prefix, int limit = 5) const;

private:
    TrieNode* root_;

    // Traverses subtree starting at 'current' to collect suggestions.
    void collectSuggestions(TrieNode* current,
                            string& currentWord,
                            vector<string>& suggestions,
                            int limit) const;
};

#endif // TRIE_H
