//
// Created by Walker on 2022/7/22.
//
#include "../ds/priority_queue.h"

#include <random>

CompareResult compare(const Slice& x, const Slice& y) {
    int res = memcmp(x.begin, y.begin, ID);
    if (res < 0) return LESS;
    else if (res == 0) return EQUAL;
    else return LARGE;
}

void test_priority_queue() {
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    PriorityQueue<100> q(ID, compare);
    for (int i = 0; i < 100; i++) {
        Id j = uid(dre);
        q.Push({(Ptr)&j, ID});
        if (q.Full()) {
            while (!q.Empty()) {
                Slice x = q.Pop();
                cout << *(Id*)x.begin << " ";
            } cout << endl;
        }
    }
}

int main() {
    test_priority_queue();
}