//
// Created by 董文阔 on 2022/7/15.
//

#ifndef WALKERDB_DEFINITION_H
#define WALKERDB_DEFINITION_H

using UInt = unsigned int;
using Int  = int;
using Id   = int;
using Size_t = unsigned int;
using Byte = unsigned char;
using Status = int;
using Ptr  = Byte*;

#include <iostream>
#include <cstring>
#define MAX_SLICE_SIZE 128
struct Slice {
    Ptr begin;
    Size_t size;
    Slice(Ptr begin = NULL, Size_t size = 0) {
        this->size = size;
        if (size != 0)
            memcpy(content, begin, size);
        this->begin = content;
    }

    long long GetInteger() const {
        long long x = 0;
        memcpy((Ptr)&x, begin, size);
        if ((begin[size - 1] & (1 << 7))) {
            memset((Ptr)&x + size, 0xff, sizeof(long long) - size);
        }
        return x;
    }

protected:
    Byte content[MAX_SLICE_SIZE];
};

#define ID sizeof(Id)

const Size_t PAGE_SIZE = 512;
const Id NULL_ID = -1;
const Size_t BUFFER_SIZE = 128;
const Size_t BASKET_NUM = 73;

#include <iostream>
#include <memory>
using namespace std;


enum CompareResult {
    LESS,
    EQUAL,
    LARGE
};
typedef CompareResult (*Compare)(const Slice&, const Slice&);

Size_t get_index_table_record_max_num(Size_t index_size);
Size_t get_list_table_record_max_num(Size_t record_size);
#include <cstring>

const Size_t CHAR32 = 32;
const Size_t CHAR64 = 64;
CompareResult string_compare(const Slice& a, const Slice& b);
CompareResult integer_compare(const Slice& a, const Slice& b);
#define MEMORY_PAGE_NUM 1024 * 16 * 16
#define MAX_INDEX_NUM 4
#define MAX_QUEUE_NUM 256
#define FILE_NAME "walker_db.wdb"



struct IndexPart {
    pair<Id, Size_t> range[MAX_INDEX_NUM];
    Size_t index_num = 0;
    Size_t Size() const {
        Size_t sum = 0;
        for (Int i = 0; i < index_num; i++) {
            sum += range[i].second;
        } return sum;
    }
    bool operator == (const IndexPart& part) const {
        if (index_num != part.index_num) return false;
        for (int i = 0; i < index_num; i++) {
            if (range[i].second == part.range[i].second)
                continue;
            else return false;
        } return true;
    }

    bool operator != (const IndexPart& part) const {
        return !(*this == part);
    }
};
IndexPart EmptyIndexPart();
Slice GetIndex(const Slice& record, const IndexPart& part);
#include <cstring>



const int ALREADY_EXISTS = -3;



#endif //WALKERDB_DEFINITION_H
