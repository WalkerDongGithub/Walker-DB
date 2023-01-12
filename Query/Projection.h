//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_PROJECTION_H
#define WALKERDB_PROJECTION_H


#include "Iterator/Iterable.h"
/**
 * 投影操作
 */
class ProjectionQuery : public Iterable {

    Iterable& iterable;

    IndexPart part;

public:
    friend class ProjectionQueryIterator;
    Iterator * GenerateIterator() override;
    FileSystem & FS() override;
    Size_t RecordSize() override;

    class ProjectionQueryIterator : public Iterator {
        ProjectionQuery& query;
        Iterator* iterator;
    public:
        ProjectionQueryIterator(ProjectionQuery& query): query(query), iterator(nullptr) {
            Init();
        }
        ~ProjectionQueryIterator() {
            delete iterator;
        }
        void  Init() override {
            if (iterator != nullptr) delete iterator;
            iterator = query.iterable.GenerateIterator();
        }
        bool  Null() override {
            return iterator->Null();
        }
        void  Next() override {
            iterator->Next();
        }
        Slice Get()  override {
            return GetIndex(iterator->Get(), query.part);
        }
    };

    ProjectionQuery(Iterable& iterable, const IndexPart& part) : iterable(iterable), part(part) {}



};



#endif //WALKERDB_PROJECTION_H
