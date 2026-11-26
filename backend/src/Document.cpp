/*
 * Document.cpp — Implementation of the Document class.
 *
 * Key design decisions:
 *   - We read the entire file into a string (documents are small text files).
 *   - Tokenization is a single O(n) pass: we walk character by character,
 *     building words from alphanumeric runs and discarding everything else.
 *   - Normalization happens during tokenization (not as a separate pass)
 *     to avoid iterating over the content twice.
 */

#include "Document.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

// ════════════════════════════════════════════════════════════════
// Constructor
// ════════════════════════════════════════════════════════════════

Document::Document(int id, const string& filename) {
    id_ = id;
    filename_ = filename;
}

// ════════════════════════════════════════════════════════════════
// Core Operations
// ════════════════════════════════════════════════════════════════

/*
 * loadFromFile — Reads the entire file into the content_ string.
 *
 * Uses a stringstream to read the whole file at once.
 * This is efficient for small text files (which is our use case).
 *
 * Time:  O(n) where n = file size
 * Space: O(n) for storing the content
 */
bool Document::loadFromFile(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "[ERROR] Cannot open file: " << filepath << "\n";
        return false;
    }

    // Read entire file into a string using stringstream
    stringstream buffer;
    buffer << file.rdbuf();
    content_ = buffer.str();

    file.close();
    return true;
}

/*
 * tokenize — Splits content into normalized words.
 *
 * Algorithm (single-pass, O(n)):
 *   1. Walk through each character of content_.
 *   2. If the character is alphanumeric, append its lowercase form
 *      to the current word being built.
 *   3. If it's NOT alphanumeric (space, punctuation, newline, etc.),
 *      that marks the end of a word — save it and start a new one.
 *   4. After the loop, don't forget the last word if content doesn't
 *      end with a separator.
 *
 * Example:
 *   "Hello, World! C++ is FUN."
 *    → ["hello", "world", "c", "is", "fun"]
 *
 * Note: "C++" becomes "c" because '+' is not alphanumeric.
 * This is acceptable for a mini search engine.
 *
 * Time:  O(n) — single pass over content
 * Space: O(n) — storing the resulting words
 */
void Document::tokenize() {
    words_.clear();

    string currentWord;

    for (char c : content_) {
        if (isAlphaNum(c)) {
            // Part of a word — append lowercase version
            currentWord += toLower(c);
        } else {
            // Separator found — save the word if non-empty
            if (!currentWord.empty()) {
                words_.push_back(currentWord);
                currentWord.clear();
            }
        }
    }

    // Don't forget the last word (if content doesn't end with a separator)
    if (!currentWord.empty()) {
        words_.push_back(currentWord);
    }
}

// ════════════════════════════════════════════════════════════════
// Accessors
// ════════════════════════════════════════════════════════════════

int Document::getId() const {
    return id_;
}

const string& Document::getFilename() const {
    return filename_;
}

const string& Document::getContent() const {
    return content_;
}

const vector<string>& Document::getWords() const {
    return words_;
}

int Document::getWordCount() const {
    return static_cast<int>(words_.size());
}

/*
 * getPreview — Returns the first maxLength characters of the content.
 *
 * If the content is longer than maxLength, we cut at the last space
 * before the limit to avoid breaking words, then append "..."
 */
string Document::getPreview(int maxLength) const {
    if (static_cast<int>(content_.size()) <= maxLength) {
        return content_;
    }

    // Find the last space before maxLength to avoid cutting mid-word
    string preview = content_.substr(0, maxLength);
    size_t lastSpace = preview.rfind(' ');

    if (lastSpace != string::npos && lastSpace > 0) {
        preview = preview.substr(0, lastSpace);
    }

    return preview + "...";
}

// ════════════════════════════════════════════════════════════════
// Helper Functions (private, static)
// ════════════════════════════════════════════════════════════════

/*
 * toLower — Converts a single character to lowercase.
 *
 * We implement this manually instead of using tolower
 * to keep it simple and avoid locale issues.
 */
char Document::toLower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');  // ASCII offset: 'a' - 'A' = 32
    }
    return c;
}

/*
 * isAlphaNum — Checks if a character is a letter (a-z, A-Z) or digit (0-9).
 *
 * We use manual range checks instead of isalnum
 * to avoid locale-dependent behavior.
 */
bool Document::isAlphaNum(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}
