// Document.cpp — File loading and single-pass tokenization.

#include "Document.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Document::Document(int id, const string& filename) {
    id_ = id;
    filename_ = filename;
}

bool Document::loadFromFile(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "[ERROR] Cannot open file: " << filepath << "\n";
        return false;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    content_ = buffer.str();

    file.close();
    return true;
}

void Document::tokenize() {
    words_.clear();

    string currentWord;

    for (char c : content_) {
        if (isAlphaNum(c)) {
            currentWord += toLower(c);
        } else {
            if (!currentWord.empty()) {
                words_.push_back(currentWord);
                currentWord.clear();
            }
        }
    }

    if (!currentWord.empty()) {
        words_.push_back(currentWord);
    }
}

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

string Document::getPreview(int maxLength) const {
    if (static_cast<int>(content_.size()) <= maxLength) {
        return content_;
    }

    // Cut at the last space to avoid splitting a word.
    string preview = content_.substr(0, maxLength);
    size_t lastSpace = preview.rfind(' ');

    if (lastSpace != string::npos && lastSpace > 0) {
        preview = preview.substr(0, lastSpace);
    }

    return preview + "...";
}

char Document::toLower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

bool Document::isAlphaNum(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}
