//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_DOUBLE_LINKED_LIST_H
#define WALKERDB_DOUBLE_LINKED_LIST_H


#include <vector>
#include "../definition.h"
#include "MemoryManager.h"
template < class T > struct ListNode {
    T t;
    Id prev;
    Id next;
};

template < class T, Id N > class DoubleLinkedList {
public:

    Memory<ListNode<T>, N>* l;
    Id head;
    Id tail;
    DoubleLinkedList(): l(nullptr) {
        head = tail = NULL_ID;
    }

    DoubleLinkedList(Memory<ListNode<T>, N>* l): l(l) {
        head = tail = NULL_ID;
    }

    Id Create(const T& t, Id nxt, Id pre) {
        Id new_t = l->Create();
        if (new_t == NULL_ID) {
            return NULL_ID;
        }
        (*l)[new_t].t = t;
        (*l)[new_t].next = nxt;
        (*l)[new_t].prev = pre;
        return new_t;
    }
    bool PushFront(const T& t, Id& i) {
        i = Create(t, head, NULL_ID);
        if (i == NULL_ID) {
            return false;
        }
        if (head == NULL_ID) {
            tail = i;
        } else {
            (*l)[head].prev = i;
        } head = i;
        return true;
    }

    bool PopFront() {
        if (head == NULL_ID) return false;
        l->Remove(head);
        head = (*l)[head].next;
        if (head == NULL_ID) {
            tail = NULL_ID;
        } else {
            (*l)[head].prev = NULL_ID;
        }
        return true;
    }
    bool PopBack(T& x) {
        if (tail == NULL_ID) return false;
        x = (*l)[tail].t;
        l->Remove(tail);
        tail = (*l)[tail].prev;
        if (tail == NULL_ID) {
            head = NULL_ID;
        } else {
            (*l)[tail].next = NULL_ID;
        }
        return true;
    }
    bool PopBack() {
        if (tail == NULL_ID) return false;
        l->Remove(tail);
        tail = (*l)[tail].prev;
        if (tail == NULL_ID) {
            head = NULL_ID;
        } else {
            (*l)[tail].next = NULL_ID;
        }
        return true;
    }
    bool Remove(Id i) {
        if (i == NULL_ID) return false;
        else if (i == head) return PopFront();
        else if (i == tail) return PopBack();
        else {
            if ((*l)[i].next != NULL_ID) (*l)[(*l)[i].next].prev = (*l)[i].prev;
            if ((*l)[i].prev != NULL_ID) (*l)[(*l)[i].prev].next = (*l)[i].next;
            l->Remove(i);
            return true;
        }
    }


    bool MoveToFront(Id i) {
        if (i == NULL_ID) return false;
        if (i == head) return true;
        else {
            if ((*l)[i].next != NULL_ID) (*l)[(*l)[i].next].prev = (*l)[i].prev;
            if ((*l)[i].prev != NULL_ID) (*l)[(*l)[i].prev].next = (*l)[i].next;
            (*l)[head].prev = i;
            (*l)[i].next = head;
            head = i;
            return true;
        }
    }

    void Debug() {
        for (Id i = head; i != NULL_ID; i = (*l)[i].next) {
            cout << "(" << i << ", " << (*l)[i].t.key << ")" << ", ";
        } cout << "." << endl;
    }

    vector<T> Traverse() const {
        Id p = head;
        vector<T> res;
        while (p != NULL_ID) {
            res.push_back((*l)[p].t);
            p = (*l)[p].next;
        } return move(res);
    }

};




#endif //WALKERDB_DOUBLE_LINKED_LIST_H
