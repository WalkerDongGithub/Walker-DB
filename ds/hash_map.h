//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_HASH_MAP_H
#define WALKERDB_HASH_MAP_H

#include "../definition.h"
#include "double_linked_list.h"

template < class K, class V >
struct Entry {
    K key;
    V value;
};

template < class K, class V, Id SIZE, Id BASKET_NUM > class HashMap {

    Memory<ListNode<Entry<K, V>>, SIZE>& h;
    DoubleLinkedList<Entry<K, V>, SIZE> hash_baskets[BASKET_NUM];
public:
    HashMap(Memory<ListNode<Entry<K, V>>, SIZE>& h): h(h) {
        for (auto& x : hash_baskets) {
            x = DoubleLinkedList<Entry<K, V>, SIZE>(&h);
        }
    }
    bool Insert(const K& k, const V& v) {
        Id hash = Hash(k);
        Id x;
        if (Find(k, x)) return false;
        return hash_baskets[hash].PushFront({k, v}, x);
    }

    bool Remove(const K& k) {
        Id hash = Hash(k);
        for (Id i = hash_baskets[hash].head; i != NULL_ID; i = h[i].next) {
            if (h[i].t.key == k) {
                hash_baskets[hash].Remove(i);
                return true;
            }
        } return false;
    }

    bool Find(const K& k, V& v) {
        Id hash = Hash(k);
        for (Id i = hash_baskets[hash].head; i != NULL_ID; i = h[i].next) {
            if (h[i].t.key == k) {
                v = h[i].t.value;
                hash_baskets[hash].MoveToFront(i);
                return true;
            }
        } return false;
    }



    Id Hash(const K& k) {
        return k % BASKET_NUM;
    }

    void Debug() {
        for (Id i = 0; i < BASKET_NUM; i++) {
            hash_baskets[i].Debug();
        }
    }


};




#endif //WALKERDB_HASH_MAP_H
