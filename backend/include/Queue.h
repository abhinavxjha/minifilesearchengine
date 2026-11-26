// Queue.h — Singly-linked FIFO queue for document indexing tasks.

#ifndef QUEUE_H
#define QUEUE_H

#include <string>

using namespace std;

struct IndexTask {
    int docId;
    string filename;
    string filepath;

    IndexTask() {
        docId = -1;
    }
    IndexTask(int id, const string& fname, const string& fpath) {
        docId = id;
        filename = fname;
        filepath = fpath;
    }
};

struct QueueNode {
    IndexTask task;
    QueueNode* next;

    QueueNode(const IndexTask& t) {
        task = t;
        next = nullptr;
    }
};

class Queue {
public:
    Queue();
    ~Queue();

    void enqueue(int docId, const string& filename, const string& filepath);
    IndexTask dequeue();
    bool isEmpty() const;
    void clear();

private:
    QueueNode* front_;
    QueueNode* rear_;
};

#endif // QUEUE_H
