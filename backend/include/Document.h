// Document.h — Loads and tokenizes a text document.

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
        string getPreview(int maxLength = 200) const;

    private:
        int id_;
        string filename_;
        string content_;
        vector<string> words_;

        static char toLower(char c);
        static bool isAlphaNum(char c);
    };

#endif // DOCUMENT_H
