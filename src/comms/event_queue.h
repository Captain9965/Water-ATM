#pragma once 

#include "common/common.h"

/*
create a generic queue class that can be used 
to handle both insertion and removal of custom types, as well as
as well as to accept any number of predefined events
Do not use RTOS queues, instead implement a simple circular buffer queue that can be used in both ISR and main loop contexts.
use template class to create a generic queue that can handle any type of event, and use a mutex to protect the queue from concurrent access.
*/  

template <typename T, size_t N>
class EventQueue {
    public:
        EventQueue() : head(0), tail(0), count(0) {}    

        bool enqueue(const T& item) {
            if (count == N) {
                return false;  // Queue is full
            }
            buffer_[tail] = item;
            tail = (tail + 1) % N;
            count++;
            return true;
        }

        bool peek(T& item) const {
            if (count == 0) {
                return false;  // Queue is empty
            }
            item = buffer_[head];
            return true;
        }

        bool dequeue(T& item) {
            if (count == 0) {
                return false;  // Queue is empty
            }
            item = buffer_[head];
            head = (head + 1) % N;
            count--;
            return true;
        }

    private:
        T buffer_[N];
        size_t head;
        size_t tail;
        size_t count;
};
