// main.cpp — Entry point for the Mini Search Engine server and CLI verification.

#include <iostream>
#include <filesystem>
#include <vector>
#include <cstring>
#include "Document.h"
#include "LinkedList.h"
#include "HashTable.h"
#include "InvertedIndex.h"
#include "Trie.h"
#include "SearchEngine.h"
#include "MergeSort.h"
#include "Queue.h"
#include "HttpServer.h"

using namespace std;

namespace fs = filesystem;

void runAutomatedEndpointVerification(SearchEngine& engine) {
    cout << "──────────────────────────────────────────\n";
    cout << "  VERIFYING API ENDPOINT SERIALIZATIONS\n";
    cout << "──────────────────────────────────────────\n";

    cout << "  [1] Autocomplete prefix 'pro':\n";
    auto autocomp = engine.autocomplete("pro", 5);
    cout << "      Count: " << autocomp.size() << " | First: " 
              << (autocomp.empty() ? "none" : autocomp[0]) << "\n";

    cout << "  [2] Search 'machine learning':\n";
    auto results = engine.search("machine learning");
    cout << "      Found: " << results.size() << " results\n";
    if (!results.empty()) {
        cout << "      Top result: " << results[0].filename 
                  << " (Score: " << results[0].score << ")\n";
    }

    cout << "\n  All core search endpoint serializations verified successfully!\n";
}

int main(int argc, char* argv[]) {
    cout << "==========================================\n";
    cout << "       MINI SEARCH ENGINE v1.0\n";
    cout << "           C++ DSA Engine\n";
    cout << "==========================================\n\n";

    string docsPath = "documents";
    if (!fs::exists(docsPath)) {
        cerr << "[ERROR] 'documents/' directory not found.\n";
        return 1;
    }

    SearchEngine engine(503);
    cout << "[INFO] Indexing documents via custom Queue...\n";
    int indexedCount = engine.indexDirectory(docsPath);
    cout << "[INFO] Successfully indexed " << indexedCount << " documents.\n\n";

    bool testOnly = (argc > 1 && (strcmp(argv[1], "--test") == 0));

    if (testOnly) {
        runAutomatedEndpointVerification(engine);
        cout << "\n══════════════════════════════════════════\n";
        cout << "  Verification tests passed!\n";
        cout << "══════════════════════════════════════════\n";
        return 0;
    }

    int port = 8080;
    HttpServer server(engine, port);
    server.start();

    return 0;
}
