//
// Created by 董文阔 on 2022/7/23.
//

#ifndef WALKERDB_SUB_H
#define WALKERDB_SUB_H



#include "Iterator/Iterable.h"
#include "Sort.h"
/**
 * 差操作，实际是对两表进行了一次排序，然后从头扫描
 */
class SubQuery : public Iterable {

    Iterable& a, &b;
    Compare compare;
    IndexPart Part() {
        return {{{0, a.RecordSize()}}, 1};
    }
public:
    friend class SubQueryIterator;
    Iterator * GenerateIterator() override;

    FileSystem & FS() override;
    Size_t RecordSize() override;
    class SubQueryIterator : public Iterator {
        SubQuery& query;
        Iterator* a;
        Iterator* b;
        SortQuery sa, sb;
    public:
        SubQueryIterator(SubQuery& query):
                query(query),
                sa(query.a, query.compare, query.Part()),
                sb(query.b, query.compare,query.Part()),
                a(nullptr), b(nullptr) {
            Init();
        }
        ~SubQueryIterator() {
            if (a) delete a;
            if (b) delete b;
        }
        void  Init() override {
            if (a) delete a;
            if (b) delete b;
            a = sa.GenerateIterator();
            b = sb.GenerateIterator();
            Cal();
        }
        bool  Null() override {
            return a->Null();
        }
        void  Next() override {
            a->Next();
            Cal();
        }
        Slice Get()  override {
            return a->Get();
        }


        void Cal() {
            while (!a->Null() && !b->Null()) {
                Slice a_val = a->Get();
                Slice b_val = b->Get();
                switch (query.compare(a_val, b_val)) {
                    case LESS: {
                        return;
                    }
                    case LARGE: {
                        b->Next();
                        break;
                    }
                    case EQUAL: {
                        a->Next();
                        break;
                    }
                }
            }
        }

    };

    SubQuery(Iterable& a, Iterable& b, Compare compare)
            : a(a), b(b), compare(compare) {
        SetIndexPart(Part());
    }


};




#endif //WALKERDB_SUB_H
