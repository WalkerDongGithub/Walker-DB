//
// Created by 董文阔 on 2022/7/17.
//

#ifndef WALKERDB_SIMPLEBUFFER_H
#define WALKERDB_SIMPLEBUFFER_H

#include "Buffer.h"

/**
 * 不使用置换算法，即内存即磁盘。页号完全相等，只用于debug。
 */
class SimpleBuffer : public Buffer {

public:
    Id Get(Id i, Id& replace_key, Id& replace_value, bool& miss) override {
        replace_key = NULL_ID;
        replace_value = NULL_ID;
        miss = false;
        return i;
    }
    vector<Id> GetAllPagesInBuffer() const override {
        return vector<Id>();
    }
};


#endif //WALKERDB_SIMPLEBUFFER_H
