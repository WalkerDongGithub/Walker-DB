//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_BUFFERFILESYSTEM_H
#define WALKERDB_BUFFERFILESYSTEM_H

#include "AbstractFileSystem.h"
#include "../Disk/Disk.h"
#include "../Buffer/Buffer.h"

/**
 * File System 完成最基本的 read 和 write 的接口，之后所有的磁盘访问都通过这两个接口实现。
 * 由更高级的层次来判断一个文件到底是哪类文件，从而使用File System管理文件系统。
 * File System只负责进行基本的磁盘读写操作。
 * File System 本质上是将磁盘系统封装为一块大型内存用于读写，
 * 其页面置换策略应可以保证一次表的查询处理过程中，几乎所有的需要处理的部分在同一时间都应在内存当中。
 * 其所使用的Disk形式、页面置换策略均通过引用传递进系统。
 *
 * 2022/09/16 实现了全部写回，todo: 未测试。
 */
#include "../../ds/stack.h"
class BufferFileSystem : public FileSystem {
    Disk& disk;
    Buffer& buffer;
    Page page[BUFFER_SIZE];
    bool change_flag[BUFFER_SIZE];
public:
    BufferFileSystem(Disk& disk, Buffer& buffer): disk(disk), buffer(buffer) {}

    const Page& Read(Id i) override {
        Id page_id = Get(i);
        return page[page_id];
    }

    Status Write(Id i, const Slice& slice, Int begin) override {
        Id page_id = Get(i);
        memcpy(page[page_id].GetData() + begin, slice.begin, slice.size);
        change_flag[page_id] = true;
        return OK;
    }

    Id New() override {
        Id id;
        if (e_s.Empty()) {
            Status error = disk.New(id);
            if (error != OK) throw ERROR_DISK;
            Id page_id = Get(id);
            change_flag[page_id] = false;
        } else {
            id = e_s.Pop();
            Id page_id = Get(id);
            memset(page[page_id].GetData(), 0xff, PAGE_SIZE);
            change_flag[page_id] = false;
        } return id;
    }

    void Delete(Id id) override {
        e_s.Push(id);
    }


    Size_t Size() override {
        return disk.Size();
    }

    Status WriteAllBack() override {
        vector<Id> pageInBuffer = buffer.GetAllPagesInBuffer();
        for (auto x : pageInBuffer) {
            if (change_flag[x]) {
                Id replace_key, replace_value;
                bool miss;
                Id i = buffer.Get(x, replace_key, replace_value, miss);
                if (miss) throw 1235;
                disk.Write(x, page[i]);
            }
        } return OK;
    }

private:

    Id Get(Id i) {
        if (i == NULL_ID) {
            throw NULL_ID;
        }
        Id replace_key, replace_value;
        bool miss;
        Id page_id = buffer.Get(i, replace_key, replace_value, miss);

        if (miss) {
            if (replace_key != NULL_ID) {
                if (change_flag[replace_value]) {
                    if (disk.Write(replace_key, page[replace_value]) != OK) {
                        throw ERROR_OUT_OF_BOUND;
                    }
                }
            }
            if (disk.Read(i, page[page_id]) != OK) {
                throw ERROR_OUT_OF_BOUND;
            }
            change_flag[page_id] = false;
        }
        return page_id;
    }


    Stack<Id, MEMORY_PAGE_NUM> e_s;

};


#endif //WALKERDB_BUFFERFILESYSTEM_H
