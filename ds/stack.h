//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_STACK_H
#define WALKERDB_STACK_H
#include "../definition.h"
template< class T, unsigned long N > class Stack {

    T s[N];
    Id _top;

public:
    Stack() {
        _top = 0;
    }
    bool Push(const T& t) {
        if (Full()) { return false; }
        s[_top++] = t;
        return true;
    }
    T Pop() {
        return s[--_top];
    }
    bool Empty() const {
        return _top == 0;
    }
    bool Full() const {
        return _top == N;
    }
    T Top() {
        return s[_top - 1];
    }

};



#endif //WALKERDB_STACK_H
