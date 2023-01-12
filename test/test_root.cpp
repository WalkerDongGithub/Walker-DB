//
// Created by Walker on 2022/7/19.
//

#include "../table/db_file.h"

#include "file_system/MemoryFileSystem.h"
MemoryFileSystem fs;
void test_root() {

    RootFile root(fs);

    for (int i = 0; i < 10; i++) {
        string x = "i_" + to_string(i);
        while (x.size() != CHAR16) {
            x.push_back(' ');
        }
        root.CreateTable(x.c_str());
    }
    for (int i = 0; i < 10; i++) {
        string x = "i_" + to_string(i);
        while (x.size() != CHAR16) {
            x.push_back(' ');
        }
        cout << root.QueryTableLoc(x.c_str()) << " ";
    }
    cout << endl;
}
int main() {
    test_root();
}