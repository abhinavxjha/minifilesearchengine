/*
 * Queue.cpp — Implementation of Custom Linked List Queue.
 */

#include "Queue.h"
#include <iostream>

using namespace std;

Queue::Queue() {
    front_ = nullptr;
    rear_ = nullptr;
}

Queue::~Queue() {
    clear();
}

void Queue::enqueue(int docId, const string& filename, const string& filepath) {
    QueueNode* newNode = new QueueNode(IndexTask(docId, filename, filepath));

    if (rear_ == nullptr) {
        front_ = rear_ = newNode;
    } else {
        rear_->next = newNode;
        rear_ = newNode;
    }
}

IndexTask Queue::dequeue() {
    if (isEmpty()) {
        cerr << "[WARNING] Attempted to dequeue from empty queue.\n";
        return IndexTask();
    }

    QueueNode* temp = front_;
    IndexTask task = temp->task;

    front_ = front_->next;
    if (front_ == nullptr) {
        rear_ = nullptr;
    }

    delete temp;
    return task;
}

bool Queue::isEmpty() const {
    return front_ == nullptr;
}

void Queue::clear() {
    while (!isEmpty()) {
        dequeue();
    }
}
