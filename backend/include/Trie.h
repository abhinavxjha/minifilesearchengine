// Trie.h — ASCII prefix tree for autocomplete suggestions.

#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

using namespace std;

struct TrieNode {
    static const int ALPHABET_SIZE = 128;
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;

    TrieNode();
    ~TrieNode();
};

class Trie {
public:
    Trie();
    ~Trie();

    void insert(const string& word);
    vector<string> getSuggestions(const string& prefix, int limit = 5) const;

private:
    TrieNode* root_;

    void collectSuggestions(TrieNode* current,
                            string& currentWord,
                            vector<string>& suggestions,
                            int limit) const;
};

#endif // TRIE_H
