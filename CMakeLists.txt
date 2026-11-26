cmake_minimum_required(VERSION 3.15)
project(MiniSearchEngine LANGUAGES CXX)

# ── C++17 Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Source files ────────────────────────────────────────────────
set(SOURCES
    backend/src/main.cpp
    backend/src/Document.cpp
    backend/src/LinkedList.cpp
    backend/src/HashTable.cpp
    backend/src/Trie.cpp
    backend/src/InvertedIndex.cpp
    backend/src/MergeSort.cpp
    backend/src/Queue.cpp
    backend/src/SearchEngine.cpp
    backend/src/HttpServer.cpp
)

# ── Executable ──────────────────────────────────────────────────
add_executable(search_engine ${SOURCES})

# ── Include path ────────────────────────────────────────────────
target_include_directories(search_engine PRIVATE backend/include)

# ── Platform: link Winsock on Windows ───────────────────────────
if(WIN32)
    target_link_libraries(search_engine PRIVATE ws2_32)
endif()
