//
// Created by 董文阔 on 2022/7/17.
//

#include "table_type/index_table/BPlusTreeIndexTable.h"
#include "../fs/file_system/MemoryFileSystem.h"

static MemoryFileSystem fs;

CompareResult compare(const Slice& a, const Slice& b) {
    if (*(Id*)a.begin < *(Id*)b.begin) return LESS;
    else if (*(Id*)a.begin == *(Id*)b.begin) return EQUAL;
    else return LARGE;
}

Slice idx(Id* i) {
    return {(Ptr)i, ID};
}

#include <random>


static int test_right_insert_and_right_remove() {
    cout << "test right insert and right remove" << endl;
    Id x = fs.New();
    IndexTable* table = IndexTable::Create(BPlusTreeIndexTableType, ID, fs, x, 0, compare);
    int n = 150;
    for (Id i = 0; i < n; i++) {
        Id y = NULL_ID;
        table->Insert(idx(&i), Slice((Ptr)&y, ID));
        table->Debug();
    }
    for (Id i = 0; i < n; i++) {
        table->Remove(idx(&i));
        table->Debug();
    }
    return 0;
}

static void test_right_insert_and_left_remove() {
    cout << "test right insert and left remove" << endl;
    Id x = fs.New();
    IndexTable* table = IndexTable::Create(BPlusTreeIndexTableType, ID, fs, x, 0, compare);
    int n = 150;
    for (Id i = 0; i < n; i++) {
        Id y = NULL_ID;
        table->Insert(idx(&i), Slice((Ptr)&y, ID));
        table->Debug();
    }
    for (Id i = n; i > 0; i--) {
        table->Remove(idx(&i));
        table->Debug();
    }
}

static void test_random() {

    cout << "test random insert and remove" << endl;
    Id id = fs.New();
    IndexTable* table = IndexTable::Create(BPlusTreeIndexTableType, ID, fs, id, 0, compare);
    default_random_engine dre(time(0));
    uniform_int_distribution<Int> b(0, 1);
    uniform_int_distribution<Id> uid(0, 1000000);
    for (int j = 0; j < 1000; j++) {
        Id x = uid(dre);
        bool a = b(dre);
        if (a) {
            Id y = NULL_ID;
            table->Insert(idx(&x), Slice((Ptr)&y, ID));
            table->Debug();
        }
        else {
            table->Remove(idx(&x));
            table->Debug();
        }
    }
    table->Debug();
}
int main() {
    try {
        test_right_insert_and_left_remove();
        test_right_insert_and_right_remove();
//        test_random();
    } catch (long e) {
        return 1;
    }
    return 0;
}