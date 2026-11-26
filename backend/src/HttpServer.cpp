// HttpServer.cpp — Lightweight HTTP/1.1 server for search API and static UI.

#include <winsock2.h>
#include <ws2tcpip.h>
#include "HttpServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;

HttpServer::HttpServer(SearchEngine& engine, int port) : engine_(engine) {
    port_ = port;
    isRunning_ = false;
    serverSocket_ = INVALID_SOCKET;
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "[ERROR] WSAStartup failed with error: " << result << "\n";
    }
}

HttpServer::~HttpServer() {
    stop();
    WSACleanup();
}

void HttpServer::start() {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        cerr << "[ERROR] Could not create socket: " << WSAGetLastError() << "\n";
        return;
    }
    serverSocket_ = static_cast<uintptr_t>(s);

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<u_short>(port_));

    if (bind(s, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "[ERROR] Bind failed on port " << port_ << ": " << WSAGetLastError() << "\n";
        closesocket(s);
        serverSocket_ = INVALID_SOCKET;
        return;
    }

    if (listen(s, 10) == SOCKET_ERROR) {
        cerr << "[ERROR] Listen failed: " << WSAGetLastError() << "\n";
        closesocket(s);
        serverSocket_ = INVALID_SOCKET;
        return;
    }

    isRunning_ = true;
    cout << "========================================================\n";
    cout << "  Mini Search Engine Server RUNNING\n";
    cout << "  URL: http://localhost:" << port_ << "/\n";
    cout << "  Press Ctrl+C to terminate\n";
    cout << "========================================================\n\n";

    while (isRunning_) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(s, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            if (!isRunning_) break;
            continue;
        }

        handleClient(static_cast<uintptr_t>(clientSocket));
    }
}

void HttpServer::stop() {
    isRunning_ = false;
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(static_cast<SOCKET>(serverSocket_));
        serverSocket_ = INVALID_SOCKET;
    }
}

void HttpServer::handleClient(uintptr_t clientSockPtr) {
    SOCKET client = static_cast<SOCKET>(clientSockPtr);

    char buffer[4096];
    int bytesReceived = recv(client, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        closesocket(client);
        return;
    }

    buffer[bytesReceived] = '\0';
    string requestStr(buffer);

    istringstream reqStream(requestStr);
    string method, fullPath, protocol;
    reqStream >> method >> fullPath >> protocol;

    if (method.empty() || fullPath.empty()) {
        closesocket(client);
        return;
    }

    string path = fullPath;
    map<string, string> params;

    size_t queryPos = fullPath.find('?');
    if (queryPos != string::npos) {
        path = fullPath.substr(0, queryPos);
        string queryString = fullPath.substr(queryPos + 1);

        istringstream qStream(queryString);
        string pair;
        while (getline(qStream, pair, '&')) {
            size_t eqPos = pair.find('=');
            if (eqPos != string::npos) {
                string key = pair.substr(0, eqPos);
                string val = urlDecode(pair.substr(eqPos + 1));
                params[key] = val;
            }
        }
    }

    string response = routeRequest(method, path, params);
    send(client, response.c_str(), static_cast<int>(response.size()), 0);
    closesocket(client);
}

string HttpServer::routeRequest(const string& method, 
                                const string& path, 
                                const map<string, string>& params) {
    if (method != "GET") {
        return buildResponse(405, "application/json", "{\"error\":\"Method Not Allowed\"}");
    }

    if (path == "/" || path == "/index.html") {
        return handleStaticFile("frontend/index.html", "text/html; charset=UTF-8");
    }
    if (path == "/style.css") {
        return handleStaticFile("frontend/style.css", "text/css; charset=UTF-8");
    }
    if (path == "/script.js") {
        return handleStaticFile("frontend/script.js", "application/javascript; charset=UTF-8");
    }

    if (path == "/api/search") {
        string q = params.count("q") ? params.at("q") : "";
        return handleSearch(q);
    }
    if (path == "/api/autocomplete") {
        string prefix = params.count("prefix") ? params.at("prefix") : "";
        return handleAutocomplete(prefix);
    }
    if (path == "/api/documents") {
        return handleDocuments();
    }

    return buildResponse(404, "application/json", "{\"error\":\"Endpoint Not Found\"}");
}

string HttpServer::handleSearch(const string& query) {
    if (query.empty()) {
        return buildResponse(200, "application/json", "{\"query\":\"\",\"total\":0,\"timeMs\":0.0,\"results\":[]}");
    }

    auto start = chrono::high_resolution_clock::now();
    vector<SearchResult> results = engine_.search(query);
    auto end = chrono::high_resolution_clock::now();
    double timeMs = chrono::duration<double, milli>(end - start).count();

    ostringstream ss;
    ss << "{"
       << "\"query\":\"" << escapeJson(query) << "\","
       << "\"total\":" << results.size() << ","
       << "\"timeMs\":" << timeMs << ","
       << "\"results\":[";

    for (size_t i = 0; i < results.size(); i++) {
        if (i > 0) ss << ",";
        const auto& r = results[i];
        ss << "{"
           << "\"docId\":" << r.docId << ","
           << "\"filename\":\"" << escapeJson(r.filename) << "\","
           << "\"score\":" << r.score << ","
           << "\"matchedTermsCount\":" << r.matchedTermsCount << ","
           << "\"matchedWords\":[";
        for (size_t k = 0; k < r.matchedWords.size(); k++) {
            if (k > 0) ss << ",";
            ss << "\"" << escapeJson(r.matchedWords[k]) << "\"";
        }
        ss << "],"
           << "\"snippet\":\"" << escapeJson(r.snippet) << "\""
           << "}";
    }
    ss << "]}";

    return buildResponse(200, "application/json", ss.str());
}

string HttpServer::handleAutocomplete(const string& prefix) {
    vector<string> suggestions = engine_.autocomplete(prefix, 8);

    ostringstream ss;
    ss << "{"
       << "\"prefix\":\"" << escapeJson(prefix) << "\","
       << "\"suggestions\":[";
    for (size_t i = 0; i < suggestions.size(); i++) {
        if (i > 0) ss << ",";
        ss << "\"" << escapeJson(suggestions[i]) << "\"";
    }
    ss << "]}";

    return buildResponse(200, "application/json", ss.str());
}

string HttpServer::handleDocuments() {
    const auto& docs = engine_.getIndex().getAllDocuments();

    ostringstream ss;
    ss << "{\"documents\":[";
    for (size_t i = 0; i < docs.size(); i++) {
        if (i > 0) ss << ",";
        const auto& d = docs[i];
        ss << "{"
           << "\"id\":" << d.getId() << ","
           << "\"filename\":\"" << escapeJson(d.getFilename()) << "\","
           << "\"wordCount\":" << d.getWordCount() << ","
           << "\"preview\":\"" << escapeJson(d.getPreview(100)) << "\""
           << "}";
    }
    ss << "]}";

    return buildResponse(200, "application/json", ss.str());
}

string HttpServer::handleStaticFile(const string& filepath, const string& contentType) {
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        return buildResponse(404, "text/plain", "File not found: " + filepath);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buildResponse(200, contentType, buffer.str());
}

string HttpServer::buildResponse(int statusCode, 
                                 const string& contentType, 
                                 const string& body) {
    string statusText = "Internal Server Error";
    if (statusCode == 200) statusText = "OK";
    else if (statusCode == 404) statusText = "Not Found";
    else if (statusCode == 405) statusText = "Method Not Allowed";

    ostringstream ss;
    ss << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    ss << "Content-Type: " << contentType << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Access-Control-Allow-Methods: GET, OPTIONS\r\n";
    ss << "Access-Control-Allow-Headers: Content-Type\r\n";
    ss << "Connection: close\r\n\r\n";
    ss << body;

    return ss.str();
}

string HttpServer::urlDecode(const string& in) {
    string out;
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 2 < in.size()) {
                int hexVal = 0;
                istringstream hexStream(in.substr(i + 1, 2));
                if (hexStream >> hex >> hexVal) {
                    out += static_cast<char>(hexVal);
                    i += 2;
                } else {
                    out += '%';
                }
            } else {
                out += '%';
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return out;
}

string HttpServer::escapeJson(const string& s) {
    ostringstream o;
    for (char c : s) {
        switch (c) {
            case '"':  o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b";  break;
            case '\f': o << "\\f";  break;
            case '\n': o << "\\n";  break;
            case '\r': o << "\\r";  break;
            case '\t': o << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) >= 32) {
                    o << c;
                }
        }
    }
    return o.str();
}
