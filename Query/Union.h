//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_UNION_H
#define WALKERDB_UNION_H

#include "Iterator/Iterable.h"

/**
 * 这里的并是对两表简单拼接，没有进行去重复处理。
 */
class UnionQuery : public Iterable {

    Iterable& a, &b;

public:
    friend class UnionQueryIterator;
    Iterator * GenerateIterator() override;

    FileSystem & FS() override;
    Size_t RecordSize() override;
    class UnionQueryIterator : public Iterator {
        UnionQuery& query;
        Iterator* a;
        Iterator* b;
    public:
        UnionQueryIterator(UnionQuery& query): query(query), a(nullptr), b(nullptr) {
            Init();
        }
        ~UnionQueryIterator() {
            if (a) delete a;
            if (b) delete b;
        }
        void  Init() override {
            if (a) delete a;
            if (b) delete b;
            a = query.a.GenerateIterator();
            b = query.b.GenerateIterator();
        }
        bool  Null() override {
            return b->Null();
        }
        void  Next() override {
            if (!a->Null()) a->Next();
            else   b->Next();
        }
        Slice Get()  override {
            if (!a->Null()) return a->Get();
            else   return b->Get();
        }

    };

    UnionQuery(Iterable& a, Iterable& b) : a(a), b(b) {}


};




#endif //WALKERDB_UNION_H
