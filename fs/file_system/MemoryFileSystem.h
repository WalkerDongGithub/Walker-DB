//
// Created by Walker on 2022/7/19.
//

#ifndef WALKERDB_SIMPLE_FILE_SYSTEM_H
#define WALKERDB_SIMPLE_FILE_SYSTEM_H


#include "AbstractFileSystem.h"
#include "Disk/Disk.h"
#include "MemoryManager.h"
#include "DebugLog.h"
class MemoryFileSystem : public FileSystem {

    Memory<Page, MEMORY_PAGE_NUM> page;

public:

    const Page& Read(Id i) override {
        return page[i];
    }

    Status Write(Id i, const Slice& slice, Int begin) override {
        memcpy(page[i].GetData() + begin, slice.begin, slice.size);
        return OK;
    }

    Id New() override {
        return page.Create();
    }

    void Delete(Id id) override {
//        page.Remove(id);
    }

    Size_t Size() override {
        return page.Size();
    }

    Status WriteAllBack() override {
        return OK;
    }


};

#endif //WALKERDB_SIMPLE_FILE_SYSTEM_H
