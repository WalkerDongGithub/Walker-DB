//
// Created by Walker on 2022/7/22.
//

#include "table_type/record_table/QueueRecordTable.h"
#include "file_system/MemoryFileSystem.h"
#include "DebugLog.h"
#include <string>
#include <random>
MemoryFileSystem fs;
void test_queue_table() {
    Id i;
    fs.New(i);
    QueueTable table = QueueTable(
            i,
            0, ID,
            fs,
            CHAR16 + ID);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (i = 0; i < 10; i++) {
        Id x = uid(dre);
        string s = "walker_" + to_string(x);
        while (s.size() < CHAR16) s.push_back(' ');
        Byte b[128];
        memcpy(b, s.c_str(), CHAR16);
        memcpy(b + CHAR16, (Ptr)&x, ID);
        table.Push({(Ptr)&b, ID + CHAR16});
        table.Debug();
    }
    cout << endl;
    for (i = 0; i < 10; i++) {
        cout << *(Id*) (table.Pop().begin + CHAR16) << " " << endl;
        table.Debug();
    }

}


int main() {
    test_queue_table();
}