//
// Created by 董文阔 on 2022/9/23.
//
#include "table_type/WithIndexTable.h"

#include "file_system/MemoryFileSystem.h"

CompareResult UIntCompare(const Slice& a, const Slice& b) {
    if (*(Id*)a.begin < *(Id*)b.begin) return LESS;
    else if (*(Id*)a.begin == *(Id*)b.begin) return EQUAL;
    else return LARGE;
}

Slice s(Id x) {
    return Slice((Ptr)&x, ID);
}
MemoryFileSystem fs;
void test_with_index_table () {
    Id x = fs.New();
    IndexPart part = {{{0, ID}}, 1};
    WithIndexTable table(
            BPlusTreeIndexTableType, QueueRecordTableType,
            fs, ID, part, x, 0, ID, UIntCompare);

    int n = 100;
    for (int i = 0; i < n; i++) {
        table.Insert(s(i));
        table.Debug();
    }
    for (int i = 0; i < n; i++) {
        table.Insert(s(i));
        table.Debug();
    }
    for (int i = 0; i < n; i++) {
        table.Remove(s(i));
        table.Debug();
    }
    for (int i = 0; i < n; i++) {
        table.Insert(s(i));
        table.Debug();
    }
    for (int i = 0; i < n; i++) {
        table.Update(s(i));
        table.Debug();
    }

}

int main() {
    test_with_index_table();
}