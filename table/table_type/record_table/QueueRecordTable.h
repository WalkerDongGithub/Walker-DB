//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_QUEUERECORDTABLE_H
#define WALKERDB_QUEUERECORDTABLE_H

#include "page_type/list_page.h"
#include "Iterator/Iterable.h"
#include "table_type/Table.h"
#include <cstring>
class QueueRecordTable : public RecordTable {


    Id root_page, root_begin;
    FileSystem& fs;
    Size_t record_size;

public:

    QueueRecordTable(
            Id root_page, Id root_begin,
            FileSystem& fs,
            Size_t record_size):
            root_page(root_page), root_begin(root_begin),
            fs(fs), record_size(record_size) {
    }

    Id Insert(const Slice &record) override {
        auto tail = Get(TailPage());
        if (tail.PageId() == NULL_ID || tail.Size() == get_list_table_record_max_num(record_size)) {
            auto p = ListPage::New(NULL_ID, fs, record_size);
            if (tail.PageId() != NULL_ID)
                tail.Next(p.PageId());
            else
                HeadPage(p.PageId());
            TailPage(p.PageId());
            auto new_p = Get(TailPage());
            new_p.Insert(new_p.Size(), record);
            return new_p.PageId();
        } else {
            tail.Insert(tail.Size(), record);
            return tail.PageId();
        }
    }

    /**
     * 极其危险的一个接口，一定小心调用，这里没法判断 page id 是否合法
     * 这里几乎只能通过顺序迭代或者索引的形式来查询到所要删除的位置
     * @param page_id
     * @param record_id
     * @return
     */
    Int Remove(const Slice& value, Id page_id, Id record_id) override {
        auto p = Get(page_id);
        if (value.begin != NULL && memcmp(p.Record(record_id).begin, value.begin, value.size) != 0) return ERROR_CALL;
        p.Remove(record_id);
        if (page_id == HeadPage() && p.Size() == 0) {
            if (TailPage() == HeadPage()) TailPage(NULL_ID);
            HeadPage(p.Next());
        } return OK;
    }


    Int Update(const Slice& record, Id page_id, Id record_id) override {
        Get(page_id).Record(record_id, record);
        return OK;
    }


    bool Empty() override {
        return HeadPage() == NULL_ID;
    }


    Id GetByIndex(Id page_id, const Slice &key, const IndexPart &part) override {
        if (page_id == NULL_ID) return NULL_ID;
        else {
            auto p = Get(page_id);
            Size_t size = p.Size();
            for (int i = 0; i < size; i++) {
                if (memcmp(key.begin, GetIndex(p.Record(i), part).begin, key.size) == 0) {
                    return i;
                }
            } return NULL_ID;
        }
    }

    Slice GetById(Id page_id, Id record_id) override {
        return Get(page_id).Record(record_id);
    }


    void Debug() override {
        Id page_id = HeadPage();
        while (page_id != NULL_ID) {
            auto p = Get(page_id);
            for (int i = 0; i < p.Size(); i++) {
                cout << *(Id*)p.Record(i).begin << " ";
            } cout << endl;
            page_id = p.Next();
        }
    }

    Int Recycle() override {
        fs.DropTable(HeadPage(), TailPage());
        return OK;
    }


public:
    Id HeadPage() {
        Ptr p = fs.Read(root_page).GetConstData();
        Id x = *(Id*)(p + root_begin);
        if (x != NULL_ID && Get(x).Size() == 0) {
            HeadPage(Get(x).Next());
            x = HeadPage();
        } return x;
    }
private:
    void HeadPage(Id page_id) {
        fs.Write(root_page, {(Ptr)&page_id, ID}, root_begin);
    }

    Id TailPage() {
        Ptr p = fs.Read(root_page).GetConstData();
        return *(Id*)(p + root_begin + ID);
    }

    void TailPage(Id page_id) {
        fs.Write(root_page, {(Ptr)&page_id, ID}, root_begin + ID);
    }

    ListPage Get(Id page_id) {
        return ListPage(page_id, fs, record_size);
    }



public:

    /**
      * Iterator Behaviour.
      *
      */
    Iterator * GenerateIterator() override {
        return new QueueRecordTableIterator(*this);
    }
    Size_t RecordSize() override {
        return record_size;
    }
    FileSystem & FS() override {
        return fs;
    }
    class QueueRecordTableIterator : public Iterator {

        QueueRecordTable& table;
        Id page_id;
        Id record_id;

    public:

        QueueRecordTableIterator(QueueRecordTable& table): table(table) {
            Init();
        }

        void Init() override {
            page_id = table.HeadPage();
            record_id = 0;
        }

        void Next() override {
            record_id++;
            if (record_id == table.Get(page_id).Size()) {
                record_id = 0;
                page_id = table.Get(page_id).Next();
            }
        }
        bool Null() override {
            return page_id == NULL_ID;
        }

        Slice Get() override {
            return table.GetById(page_id, record_id);
        }

    };



};

#endif //WALKERDB_QUEUERECORDTABLE_H
