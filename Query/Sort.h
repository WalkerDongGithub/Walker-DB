//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_SORT_H
#define WALKERDB_SORT_H

#include "Iterator/Iterable.h"
#include "file_system/AbstractFileSystem.h"
#include "table_type/SortedTable.h"
/**
 * 排序查询，需要规定排序规则
 * todo：这里对排序规则的设计并不合理，需要重构。
 */
class SortQuery : public Iterable {

    Iterable& iterable;
    Compare compare;

public:

    SortQuery(Iterable& iterable, Compare compare, const IndexPart& part) : iterable(iterable),
    compare(compare) {
        SetIndexPart(part);
    }

    Iterator * GenerateIterator() override;
    Size_t RecordSize() override;
    FileSystem & FS() override;

    class SortQueryIterator : public Iterator {
        SortQuery& query;
        Iterator* iterator;
        SortedTable sort_table;
    public:

        SortQueryIterator(SortQuery& query):
        query(query),
        sort_table(query.iterable, query.compare, query.GetIndexPart()) {
            Init();
        }
        ~SortQueryIterator() {
            if (iterator) delete iterator;
        }
        void  Init() override {
            iterator = query.iterable.GenerateIterator();
            if (query.iterable.GetIndexPart() != query.GetIndexPart()) {
                Iterator* it = sort_table.GenerateIterator();
                delete iterator;
                iterator = it;
            }
        }
        bool  Null() override {
            return iterator->Null();
        }
        void  Next() override {
            iterator->Next();
        }
        Slice Get()  override {
            return iterator->Get();
        }

    };

};


#endif //WALKERDB_SORT_H
