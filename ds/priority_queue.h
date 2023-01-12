//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_PRORITY_QUEUE_H
#define WALKERDB_PRORITY_QUEUE_H

#include "../definition.h"
#include <iostream>
template <Size_t N>
class PriorityQueue {
    Byte content[N];
    Size_t size;
    Size_t record_size;
    Compare compare;
    IndexPart part;
public:
    PriorityQueue(Size_t record_size, Compare compare, const IndexPart& part):
    size(1), record_size(record_size), compare(compare), part(part) {
    }

    void Push(const Slice& record) {
        memcpy(Get(size++), record.begin, record_size);
        UpAdjust(size - 1);
    }

    Slice Pop() {
        Slice s = {Get(1), record_size};
        memcpy(Get(1), Get(size - 1), record_size);
        size --;
        DownAdjust(1);
        return s;
    }
    bool Full() {
        return size >= N / record_size;
    }
    bool Empty() {
        return size == 1;
    }

    Slice Top() {
        return {Get(1), record_size};
    }


private:

    Ptr Get(Id i) {
        return content + i * record_size;
    }

    void UpAdjust(Id i) {
        if (i == 1) return;
        switch (compare(
                GetIndex({Get(i / 2), record_size}, part),
                GetIndex({Get(i), record_size}, part))) {
            case CompareResult::LESS:
            case CompareResult::EQUAL: {
                return;
            }
            case CompareResult::LARGE: {
                Byte bytes[128];
                memcpy(bytes, Get(i / 2), record_size);
                memcpy(Get(i / 2), Get(i), record_size);
                memcpy(Get(i), bytes, record_size);
                UpAdjust(i / 2);
            }
        }
    }

    void DownAdjust(Id i) {
        Int x;
        if (i * 2 + 1 < size) {
            switch (compare(
                    GetIndex({Get(i * 2), record_size}, part),
                    GetIndex({Get(i * 2 + 1), record_size}, part))) {
                case CompareResult::LESS:
                    x = i * 2;
                    break;
                default:
                    x = i * 2 + 1;
                    break;
            }
        } else if (i * 2 < size) {
            x = i * 2;
        } else {
            return;
        }

        switch (compare(
                GetIndex({Get(i), record_size}, part),
                GetIndex({Get(x), record_size}, part))) {
            case CompareResult::LESS:
            case CompareResult::EQUAL: {
                return;
            }
            case CompareResult::LARGE: {
                Byte bytes[128];
                memcpy(bytes, Get(x), record_size);
                memcpy(Get(x), Get(i), record_size);
                memcpy(Get(i), bytes, record_size);
                DownAdjust(x);
            }
        }

    }

    

};

#endif //WALKERDB_PRORITY_QUEUE_H
