//
// Created by Walker on 2022/7/19.
//

#ifndef WALKERDB_LIST_PAGE_H
#define WALKERDB_LIST_PAGE_H

#include "file_system/AbstractFileSystem.h"


/**
 *
 * ListPage 需要完成如下功能
 * ListPage 包括了一个页号和该页所存储的内容的访问方式的集合，注意是访问方式，
 * 访问内容并不一定在内存当中
 * 访问方式主要是两个参数，一个是所使用的文件系统的引用，一个是所存储的记录长度
 * 可以返回page id , 提供修改next，empty next，size 三个字段的接口
 * next即该页的下一页的位置，所有页按照一个链表规划
 * empty next 是当该页是一个空页时，空页中也有一个字段是用来标识下一个空页的位置的
 * 所有空页组成一个表。
 * todo：为防止大量的页浪费，这里需要优化设计，即使用一个统一或者相对统一的方式管理所有的空页
 *
 */
class ListPage {
private:

    Id page_id;
    FileSystem& fs;
    Size_t record_size;

public:

    ListPage(Id page_id, FileSystem& fs, Size_t record_size)
    : page_id(page_id), fs(fs), record_size(record_size) {}

    Id PageId() {
        return page_id;
    }

    /**
     * 这里需要检查一下next页面是否是空页，如果是，则需要删除掉该页，以减少块消耗
     * @return
     */
    Id Next() {
        auto x = fs.Read(page_id).GetConstData();
        Id next = *(Id*)x;
        do {
            auto p = Get(next, fs, record_size);
            if (p.page_id != NULL_ID && p.Size() == 0) {
                Next(p.Next());
                next = p.Next();
                fs.Delete(p.PageId());
            } else {
                return *(Id*)fs.Read(page_id).GetConstData();
            }
        } while (true);
    }

    void Next(Id id) {
        fs.Write(page_id, {(Ptr)&id, ID}, 0);
    }

    Size_t Size() {
        auto x = fs.Read(page_id).GetConstData();
        return *(Id*)(x + ID);
    }

    void Size(Int s) {
        fs.Write(page_id, {(Ptr)&s, ID}, ID);
    }

    Slice Record(Id i) {
        Ptr x = fs.Read(page_id).GetConstData();
        return { x + Offset(i), record_size };
    }

    void Record(Id i, Slice s) {
        fs.Write(page_id, s, Offset(i));
    }

    void Insert(Id i, Slice record) {
        for (Int j = Size(); j > i; j--) {
            Record(j, Record(j - 1));
        } Record(i, record);
        SizeInc();
    }

    void Remove(Id i) {
        for (Int j = i ; j < Size() - 1; j++) {
            Record(j, Record(j + 1));
        } SizeSub();
    }

    static ListPage New(Id next, FileSystem& fs, Size_t record_size) {
        Id id = fs.New();
        ListPage page = Get(id, fs, record_size);
        page.Size(0);
        page.Next(next);
        return page;
    }

private:

    static ListPage Get(Id id, FileSystem& fs, Size_t record_size) {
        return ListPage(id, fs, record_size);
    }

    Int Offset(Id i) {
        return ID + ID + i * record_size;
    }

    void SizeInc() {
        Size(Size() + 1);
    }

    void SizeSub() {
        Size(Size() - 1);
    }

};

#endif //WALKERDB_LIST_PAGE_H
