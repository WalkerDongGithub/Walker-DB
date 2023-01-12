//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_DISTINCT_H
#define WALKERDB_DISTINCT_H

#include "Iterator/Iterable.h"
#include "Sort.h"
/**
 * 去重复操作，实际是做了一次排序，然后每条重复语句只输出第一个，后边的都忽略
 */
class DistinctQuery : public Iterable {

    Iterable& iterable;
    Compare compare;

public:

    Iterator * GenerateIterator() override;
    FileSystem & FS() override;
    Size_t RecordSize() override;

    class DistinctQueryIterator : public Iterator {
        DistinctQuery& query;
        Iterator* iterator;
        SortQuery sorted;

    public:
        DistinctQueryIterator(DistinctQuery& query):
                query(query), iterator(nullptr),
                sorted(query.iterable, query.compare, query.GetIndexPart())
                {
            Init();
        }
        ~DistinctQueryIterator() {
            if (iterator) delete iterator;
        }
        void  Init() override {
            if (iterator) delete iterator;
            iterator = sorted.GenerateIterator();
        }
        bool  Null() override {
            return iterator->Null();
        }
        void  Next() override {
            Slice now_slice = iterator->Get();
            iterator->Next();
            while (!Null() && query.compare(now_slice, iterator->Get()) == EQUAL) {
                iterator->Next();
            }
        }
        Slice Get()  override {
            return iterator->Get();
        }

    };

    DistinctQuery(Iterable& iterable, Compare compare, const IndexPart& part)
            : iterable(iterable), compare(compare) {
        SetIndexPart(part);
    }


};



#endif //WALKERDB_DISTINCT_H
