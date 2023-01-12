//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_LRUBUFFER_H
#define WALKERDB_LRUBUFFER_H

#include "../../ds/hash_map.h"
#include "Buffer.h"

/**
 * LRUBuffer，使用LRU算法（哈希链表实现）作为缓冲区置换算法。
 */
class LRUBuffer : public Buffer {
public:

    LRUBuffer():
    hash_map(HashMap<Id, Id, BUFFER_SIZE, BASKET_NUM>(hash_mem)),
    double_list(DoubleLinkedList<Id, BUFFER_SIZE>(&list_mem)) {}

    Id Get(Id i, Id& replace_key, Id& replace_value, bool& miss) override {
        replace_key = replace_value = NULL_ID;
        miss = false;
//        cout << "find : " << i << ", ";
        Id ret = NULL_ID;
        if (hash_map.Find(i, ret)) {
            miss = false;
            double_list.MoveToFront(ret);
//            cout << "get : " << ret << endl;
            return ret;
        } else {
            miss = true;
            if (!double_list.PushFront(i, ret)) {
                Id x;
                double_list.PopBack(x);
                replace_key = x;
                hash_map.Find(replace_key, replace_value);
                hash_map.Remove(x);
//                cout << "Pop back " << x << ", ";
                double_list.PushFront(i, ret);
            }
            hash_map.Insert(i, ret);
//            cout << "add " << i << " in " << ret << endl;
            return ret;
        }
    }

    vector<Id> GetAllPagesInBuffer() const override {
        return move(double_list.Traverse());
    }

private:

    Memory<ListNode<Id>, BUFFER_SIZE> list_mem;
    Memory<ListNode<Entry<Id, Id>>, BUFFER_SIZE> hash_mem;
    HashMap<Id, Id, BUFFER_SIZE, BASKET_NUM> hash_map;
    DoubleLinkedList<Id, BUFFER_SIZE> double_list;

};




#endif //WALKERDB_LRUBUFFER_H
