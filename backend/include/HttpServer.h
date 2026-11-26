// HttpServer.h — Embedded HTTP/1.1 server for search API and static files.

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "SearchEngine.h"
#include <string>
#include <map>

using namespace std;

class HttpServer {
public:
    explicit HttpServer(SearchEngine& engine, int port = 8080);
    ~HttpServer();

    void start();
    void stop();

    static string urlDecode(const string& in);
    static string escapeJson(const string& s);

private:
    SearchEngine& engine_;
    int port_;
    bool isRunning_;
    uintptr_t serverSocket_;

    void handleClient(uintptr_t clientSocket);
    string routeRequest(const string& method, 
                        const string& path, 
                        const map<string, string>& params);

    string handleSearch(const string& query);
    string handleAutocomplete(const string& prefix);
    string handleDocuments();
    string handleStaticFile(const string& filepath, const string& contentType);

    static string buildResponse(int statusCode, 
                                const string& contentType, 
                                const string& body);
};

#endif // HTTPSERVER_H
