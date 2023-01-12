//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_MEMORYDISK_H
#define WALKERDB_MEMORYDISK_H


#include "Disk.h"

/**
 * 这个是为了方便debug，把内容写入内存可以在debug时发现磁盘写入的内容问题。
 *
 */

class MemoryDisk : public Disk {
private:
    Byte pages[MEMORY_PAGE_NUM][PAGE_SIZE];
    Page white_page;
    Id max_id = 0;
public:
    Status Read(Id i, Page &page) override {
        if (i >= max_id) return ERROR_OUT_OF_BOUND;
        memcpy(page.GetData(), (Ptr)pages + i * PAGE_SIZE, PAGE_SIZE);
        return OK;
    }
    Status Write(Id i, Page &page) override {
        if (i >= max_id) return ERROR_OUT_OF_BOUND;
        memcpy((Ptr)pages + i * PAGE_SIZE, page.GetData(), PAGE_SIZE);
        return OK;
    }
    Status New(Id &i) override {
        if (max_id == MEMORY_PAGE_NUM)
            throw ERROR_OUT_OF_BOUND;
        i = max_id ++;
        Write(i, white_page);
        return OK;
    }

    Size_t Size() override {
        return max_id;
    }
};

#endif //WALKERDB_MEMORYDISK_H
