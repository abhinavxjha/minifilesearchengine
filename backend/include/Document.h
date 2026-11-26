/*
 * Document.h — Represents a text document loaded from disk.
 *
 * Responsibilities:
 *   1. Read a .txt file into memory.
 *   2. Tokenize the content into individual words.
 *   3. Normalize words: convert to lowercase, strip punctuation.
 *   4. Store the word list for downstream indexing.
 *
 * This is the input pipeline — every DSA module consumes its output.
 *
 * Complexity:
 *   - Loading:    O(n) where n = file size in characters
 *   - Tokenizing: O(n) single pass over the content
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include<string>
#include<vector>
using namespace std;

class Document{
    public:
        Document(int id, const string& filename);
        bool loadFromFile(const string& filepath);
        void tokenize();

        int getId() const;
        const string& getFilename() const;
        const string& getContent() const;
        const vector<string>& getWords() const;
        int getWordCount() const;

    // Return a short preview of the document content (first N characters).
        string getPreview(int maxLength = 200) const;

    private:
        int id_;                             // Unique document identifier
        string filename_;               // Original filename (e.g., "document1.txt")
        string content_;                // Full raw text content
        vector<string> words_;     // Tokenized and normalized words

        static char toLower(char c);
        static bool isAlphaNum(char c);
    };

#endif // DOCUMENT_H
