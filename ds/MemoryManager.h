//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_MEMORYMANAGER_H
#define WALKERDB_MEMORYMANAGER_H

#include "../definition.h"
#include "stack.h"

template< class T, Id N > class Memory {

    Stack<Id, N> e_s;
    T mem[N];

public:
    Memory() {
        max_page = 1;
        size = 0;
        e_push(0);
    }
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    Id Create() {
        Id i = e_pop();
        if (i != NULL_ID) size++;
        return i;
    }
    void Remove(Id i) {
        e_push(i);
        size--;
    }

    T& operator[](Id i) {
        return mem[i];
    }

    Size_t Size() {
        return size;
    }

private:

    Id max_page;
    Size_t size;
    void e_push(Id i) {
        e_s.Push(i);
    }

    Id e_pop() {
        if (e_s.Empty()) {
            return NULL_ID;
        }
        Id ret = e_s.Pop();
        if (e_s.Empty() && max_page < N) {
            e_s.Push(max_page++);
        } return ret;
    }

};



#endif //WALKERDB_MEMORYMANAGER_H
