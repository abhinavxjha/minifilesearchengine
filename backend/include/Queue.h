/*
 * Queue.h — Custom FIFO Queue implemented via singly linked list.
 *
 * Complexity:
 *   - Enqueue: O(1)
 *   - Dequeue: O(1)
 *   - Space:   O(N) for nodes
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <string>

using namespace std;

// ── Index Task ────────────────────────────────────────────────
struct IndexTask {
    int docId;         // Assigned document ID
    string filename;   // Base file name (e.g. "document1.txt")
    string filepath;   // Full system path on disk

    IndexTask() {
        docId = -1;
    }
    IndexTask(int id, const string& fname, const string& fpath) {
        docId = id;
        filename = fname;
        filepath = fpath;
    }
};

// ── Queue Node ────────────────────────────────────────────────
struct QueueNode {
    IndexTask task;
    QueueNode* next;

    QueueNode(const IndexTask& t) {
        task = t;
        next = nullptr;
    }
};

// ── Queue Class ───────────────────────────────────────────────
class Queue {
public:
    Queue();
    ~Queue();

    // Adds a task to the back (rear) of the queue. O(1)
    void enqueue(int docId, const string& filename, const string& filepath);

    // Removes and returns the task from the front of the queue. O(1)
    IndexTask dequeue();

    // Checks if the queue contains no elements. O(1)
    bool isEmpty() const;

    // Clears all elements from the queue.
    void clear();

private:
    QueueNode* front_;  // Head of the queue
    QueueNode* rear_;   // Tail of the queue
};

#endif // QUEUE_H
