//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_QUERY_H
#define WALKERDB_QUERY_H

#include "Iterator/Iterable.h"


//typedef bool (*Filter)(const Slice&);

struct Filter {
    virtual bool f(const Slice&) = 0;
};

/**
 *
 * 选择操作
 * todo：这里需要进行优化，按理说不应该每次选择都要O(n)
 */
class SelectQuery : public Iterable {

    Iterable& iterable;
    Filter* filter;

public:

    Iterator * GenerateIterator() override;
    FileSystem & FS() override;
    Size_t RecordSize() override;

    class SelectQueryIterator : public Iterator {
        SelectQuery& query;
        Iterator* iterator;
    public:
        SelectQueryIterator(SelectQuery& query): query(query), iterator(nullptr) {
            Init();
        }
        ~SelectQueryIterator() {
            delete iterator;
        }
        void  Init() override {
            if (iterator != nullptr) delete iterator;
            iterator = query.iterable.GenerateIterator();
            while (!iterator->Null() && !query.filter->f(iterator->Get())) {
                iterator->Next();
            }
        }
        bool  Null() override {
            return iterator->Null();
        }
        void  Next() override {
            iterator->Next();
            while (!iterator->Null() && !query.filter->f(iterator->Get())) {
                iterator->Next();
            }
        }
        Slice Get()  override {
            return iterator->Get();
        }

    };

    SelectQuery(Iterable& iterable, Filter* filter) : iterable(iterable), filter(filter) {}



};




#endif //WALKERDB_QUERY_H
