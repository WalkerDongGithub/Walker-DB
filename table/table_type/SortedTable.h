//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_SORTEDTABLE_H
#define WALKERDB_SORTEDTABLE_H

#include "table_type/record_table/QueueRecordTable.h"
#include "Iterator/Iterable.h"
#include "../../ds/priority_queue.h"


/**
 * 这里SortedTable是与QueueRecordTable相耦合的，他必须是Queue才可以进行进一步的操作
 */
class SortedTable : public Iterable {

    /**
     * QueueRootTable是作为一个临时记录所有队列root的表
     * 使用之后将被回收
     */
    RecordTable* queue_root_table;
    Id queue_root_table_root_page;


    PriorityQueue<MAX_QUEUE_NUM> q;
    Iterable& origin;
    Size_t record_size;
    Compare compare;

    IndexPart part;

public:

    SortedTable(Iterable& origin, Compare compare, const IndexPart& part)
    :
    origin(origin),
    compare(compare),
    part(part),
    record_size(origin.RecordSize()),
    q(origin.RecordSize(), compare, part)
    {
        queue_root_table_root_page = FS().New();
        queue_root_table = RecordTable::Create(
                QueueRecordTableType,
                ID,
                origin.FS(),
                queue_root_table_root_page,
                0);
        SetIndexPart(part);
    }

    ~SortedTable() {
        Iterator* it = queue_root_table->GenerateIterator();
        FileSystem& fs = queue_root_table->FS();
        while (!it->Null()) {
            Id page_id = *(Id*)it->Get().begin;
            fs.DropTable(page_id, 0);
            fs.Delete(page_id);
            it->Next();
        }
        fs.DropTable(queue_root_table_root_page, 0);
        fs.Delete(queue_root_table_root_page);
        delete queue_root_table;
        delete it;
    }


private:


    QueueRecordTable GetQueueTable(Id page_id) {
        return QueueRecordTable(page_id, 0, FS(), record_size);
    }

    void Sort() {
        Iterator* iterator = origin.GenerateIterator();
        while (!iterator->Null()) {
            Slice s = iterator->Get();
            if (s.size != record_size)
                throw ERROR_RECORD_SIZE;
            if (q.Full()) {
                Id i = FS().New();
                queue_root_table->Insert({(Ptr) &i, ID});
                QueueRecordTable queue_table = GetQueueTable(i);
                while (!q.Empty()) {
                    Slice x = q.Pop();
                    queue_table.Insert({x.begin, record_size});
                }
            } q.Push(s);
            iterator->Next();
        }
        Id i = FS().New();
        queue_root_table->Insert({(Ptr) &i, ID});
        QueueRecordTable queue_table = GetQueueTable(i);
        while (!q.Empty()) {
            Slice x = q.Pop();
            Byte bytes[128];
            memcpy(bytes, x.begin, record_size);
            queue_table.Insert({bytes, record_size});
        }
        delete iterator;
        q = PriorityQueue<MAX_QUEUE_NUM>(origin.RecordSize() + ID, compare, part);
    }

public:
    /**
      * Iterator Behaviour.
      */

    Iterator * GenerateIterator() override {
        return new SortedTableIterator(*this);
    }

    Size_t RecordSize() override {
        return record_size;
    }

    FileSystem & FS() override {
        return origin.FS();
    }


    class SortedTableIterator : public Iterator {

        SortedTable& table;

    public:

        SortedTableIterator(SortedTable& table):
        table(table) {
            Init();
        }

        void Init() override {
            table.Sort();
//            table.Debug();
            Iterator* iterator = table.queue_root_table->GenerateIterator();
            while (!iterator->Null()) {
                Slice s = iterator->Get();
                if (table.q.Full())
                    throw ERROR_MEMORY;
                GetRecordIntoQueue(*(Id*)s.begin);
                iterator->Next();
            }
            delete iterator;
        }

        bool Null() override {
            return table.q.Empty();
        }

        void Next() override {
            Slice s = table.q.Pop();
            Id i = *(Id*)(s.begin + table.record_size);
            GetRecordIntoQueue(i);
        }

        Slice Get() override {
            Slice s = table.q.Top();
            return {s.begin, (Size_t)(s.size - ID)};
        }

    private:
        void GetRecordIntoQueue(Id i) {
            QueueRecordTable queue_table = table.GetQueueTable(i);
            if (!queue_table.Empty()) {
                Byte bytes[128];
                Id head = queue_table.HeadPage();
                Slice s = queue_table.GetById(head, 0);
                queue_table.Remove({NULL, 0}, head, 0);
                memcpy(bytes, s.begin, table.record_size);
                memcpy(bytes + table.record_size, (Ptr)&i, ID);
                table.q.Push({bytes, (Size_t)(table.record_size + ID)});
            }
        }
    };

public:

};

#endif //WALKERDB_SORTEDTABLE_H
