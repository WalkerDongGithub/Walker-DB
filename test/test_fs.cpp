//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_TEST_FS_CPP
#define WALKERDB_TEST_FS_CPP

#include "../fs/file_system/BufferFileSystem.h"

#include "../fs/Disk/FileDisk.h"
#include "../fs/Buffer/LRUBuffer.h"

static FileDisk disk;
static LRUBuffer buffer;

#include <random>

int test_fs() {
    BufferFileSystem fs(disk, buffer);
    Int num = 100;
    for (Id i = 0; i < num; i++) {
        fs.New();
    }

    for (Id i = 0; i < num; i++) {
        fs.Write(i, {(Ptr)&i, ID}, 0);
    }
    cout << endl;
    for (Id i = 0; i < num; i++) {
        Ptr p = fs.Read(i).GetConstData();
        cout << *(Id*)p << endl;
    } cout << endl;
    return 0;
}



#endif //WALKERDB_TEST_FS_CPP
