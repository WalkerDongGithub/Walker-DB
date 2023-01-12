//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_INTERSECTION_H
#define WALKERDB_INTERSECTION_H



#include "Iterator/Iterable.h"
#include "Sort.h"
/**
 * 交操作，实际是对两个表排序，然后两个迭代器向下比较
 */
class IntersectionQuery : public Iterable {

    Iterable& a, &b;
    Compare compare;
    IndexPart Part() {
        return {{{0, a.RecordSize()}}, 1};
    }
public:
    friend class IntersectionQueryIterator;
    Iterator * GenerateIterator() override;

    FileSystem & FS() override;
    Size_t RecordSize() override;
    class IntersectionQueryIterator : public Iterator {
        IntersectionQuery& query;
        Iterator* a;
        Iterator* b;
        SortQuery sa, sb;
        bool zigzag;
    public:
        IntersectionQueryIterator(IntersectionQuery& query):
                query(query),
                sa(query.a, query.compare, query.Part()),
                sb(query.b, query.compare,query.Part()),
                a(nullptr), b(nullptr) {
            Init();
        }
        ~IntersectionQueryIterator() {
            if (a) delete a;
            if (b) delete b;
        }
        void  Init() override {
            if (a) delete a;
            if (b) delete b;
            a = sa.GenerateIterator();
            b = sb.GenerateIterator();
            zigzag = false;
            Cal();
        }
        bool  Null() override {
            return a->Null() || b->Null();
        }
        void  Next() override {
            if (zigzag) a->Next();
            else b->Next();
            Cal();
        }
        Slice Get()  override {
            return a->Get();
        }

        void Cal() {
            while (!Null()) {
                Slice a_val = a->Get();
                Slice b_val = b->Get();
                switch (query.compare(a_val, b_val)) {
                    case LESS: {
                        a->Next();
                        break;
                    }
                    case LARGE: {
                        b->Next();
                        break;
                    }
                    case EQUAL: {
                        return;
                    }
                }
            }
        }

    };

    IntersectionQuery(Iterable& a, Iterable& b, Compare compare)
            : a(a), b(b), compare(compare) {
        SetIndexPart(Part());
    }


};




#endif //WALKERDB_INTERSECTION_H
