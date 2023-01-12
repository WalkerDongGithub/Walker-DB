//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_JOIN_H
#define WALKERDB_JOIN_H


#include "Iterator/Iterable.h"
#include "Sort.h"
/**
 * 连接操作，实际是对两表进行了排序，然后对应键拼接。
 */
class JoinQuery : public Iterable {

    Iterable& a, &b;
    IndexPart ap, bp;
    Compare compare;
public:
    friend class JoinQueryIterator;

    Iterator * GenerateIterator() override;
    FileSystem & FS() override;
    Size_t RecordSize() override;

    class JoinQueryIterator : public Iterator {
        JoinQuery& query;
        Iterator* a;
        Iterator* b;
        SortQuery sa, sb;
        bool zigzag;
    public:
        JoinQueryIterator(JoinQuery& query):
        query(query),
        sa(query.a, query.compare, query.ap),
        sb(query.b, query.compare, query.bp),
        a(nullptr), b(nullptr) {
            Init();
        }
        ~JoinQueryIterator() {
            if (a) delete a;
            if (b) delete b;
        }
        void  Init() override {
            zigzag = false;
            if (a) delete a;
            if (b) delete b;
            a = sa.GenerateIterator();
            b = sb.GenerateIterator();

            while (!Null()) {
                Slice a_key = GetIndex(a->Get(), query.ap);
                Slice b_key = GetIndex(b->Get(), query.bp);
                switch (query.compare(a_key, b_key)) {
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
        bool  Null() override {
            return a->Null() || b->Null();
        }
        void  Next() override {
            if (zigzag) a->Next();
            else b->Next();
            zigzag = !zigzag;
            while (!Null()) {
                Slice a_key = GetIndex(a->Get(), query.ap);
                Slice b_key = GetIndex(b->Get(), query.bp);
                switch (query.compare(a_key, b_key)) {
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
        Slice Get()  override {
            Slice a_val = a->Get();
            Slice b_val = b->Get();
            Byte bytes[128];
            memcpy(bytes, a_val.begin, a_val.size);
            memcpy(bytes + a_val.size, b_val.begin, b_val.size);
            return {bytes, a_val.size + b_val.size};
        }

    };

    JoinQuery(Iterable& a, Iterable& b, const IndexPart& ap, const IndexPart& bp, Compare compare)
    : a(a), b(b), ap(ap), bp(bp), compare(compare) {
        if (ap != bp) throw 234;
        SetIndexPart(ap);
    }


};




#endif //WALKERDB_JOIN_H
