//
// Created by 董文阔 on 2022/7/16.
//
#include "../ds/MemoryManager.h"

void test_create() {
    Memory<Id, 5> memory;
    for (int i = 0; i < 10; i++) {
        cout << memory.Create() << " ";
    } cout << endl;
}

void test_remove() {
    Memory<Id, 5> memory;
    for (int i = 0; i < 3; i++) cout << memory.Create() << " ";
    memory.Remove(1);
    for (int i = 0; i < 3; i++) cout << memory.Create() << " ";
    memory.Remove(4);
    for (int i = 0; i < 3; i++) cout << memory.Create() << " ";
    cout << endl;
}

int main() {
    test_create();
    test_remove();
}