#pragma once

#include <vector>

template <typename T, typename Compare = std::less<T>>
class Heap {
public:
    T top() {
        return heap[0];
    }

    void pop() {
        heap[0] = heap.back();
        heap.pop_back();
        SiftDown(0);
    }

    void push(T element) {
        heap.push_back(element);
        SiftUp(heap.size() - 1);
    }

    size_t size() {
        return heap.size();
    }

private:
    std::vector<T> heap;
    Compare compare;
    void SiftDown(size_t i) {
        while (i * 2 + 1 < heap.size()) {
            size_t left = i * 2 + 1;
            size_t right = i * 2 + 2;
            size_t next = left;
            if (right < heap.size() && compare(heap[right], heap[left])) {
                next = right;
            }
            if (!compare(heap[next], heap[i])) {
                return;
            }
            std::swap(heap[i], heap[next]);
            i = next;
        }
    }
    void SiftUp(size_t i) {
        while (true) {
            size_t next = 0;
            if (i > 0) {
                next = (i - 1) / 2;
            } else {
                next = 0;
            }
            if (!compare(heap[i], heap[next])) {
                break;
            }
            std::swap(heap[i], heap[next]);
            i = next;
        }
    }
};