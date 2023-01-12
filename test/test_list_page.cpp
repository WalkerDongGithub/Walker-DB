//
// Created by Walker on 2022/7/19.
//

#include "table_type/record_table/QueueRecordTable.h"
#include "file_system/MemoryFileSystem.h"
#include "Disk/MemoryDisk.h"
#include "Buffer/LRUBuffer.h"
#include "Buffer/SimpleBuffer.h"
#include "Disk/FileDisk.h"


MemoryFileSystem fs;

void test_insert() {
    Id start = fs.New();
    RecordTable* table = RecordTable::Create(QueueRecordTableType, ID, fs, start, 0);
    Int n = 1000;
    for (Id i = 0; i < n; i++) {
        table->Insert({(Ptr)&i, ID});
        table->Debug();
        cout << endl;
    }
    delete table;
}

void test_remove() {
    RecordTable* table = RecordTable::Create(QueueRecordTableType, ID, fs, 0, 0);
    Int n = 1000;
    for (Id i = 0; i < n; i++) {
        table->Remove();
        table->Debug();
        cout << endl;
    }
}
int main() {
    test_insert();
}