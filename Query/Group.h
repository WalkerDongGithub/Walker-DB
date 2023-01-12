//
// Created by 董文阔 on 2022/7/23.
//

#ifndef WALKERDB_GROUP_H
#define WALKERDB_GROUP_H

#include "Iterator/Iterable.h"
#include "Sort.h"
/**
 * 分组操作，实际是进行了一次排序，然后迭代器到每一类的结尾统计各类数据输出
 * 现在可以计算sum min max cnt四种
 */
class GroupQuery : public Iterable {

    Iterable& iterable;
    Compare compare;
    IndexPart part;
    IndexPart index_part;
public:
    friend class GroupQueryIterator;
    Iterator * GenerateIterator() override;
    FileSystem & FS() override;
    Size_t RecordSize() override;
    class GroupQueryIterator : public Iterator {
        GroupQuery& query;
        Iterator* iterator;
        SortQuery sorted;
        Int sum[MAX_INDEX_NUM], count[MAX_INDEX_NUM], max[MAX_INDEX_NUM], min[MAX_INDEX_NUM];
        Slice now_slice;

    public:
        GroupQueryIterator(GroupQuery& query):
                query(query), iterator(nullptr),
                sorted(query.iterable, query.compare, query.index_part)
        {
            Init();
        }
        ~GroupQueryIterator() {
            if (iterator) delete iterator;
        }
        void  Init() override {
            if (iterator) delete iterator;
            iterator = sorted.GenerateIterator();
            Cal();
        }
        bool  Null() override {
            return iterator->Null();
        }
        void  Next() override {
            if (iterator->Null()) return;
            Cal();
        }
        Slice Get()  override {
            Byte bytes[MAX_SLICE_SIZE];
            Int p = 0;
            memcpy(bytes + p, now_slice.begin, now_slice.size);
            p += now_slice.size;
            for (int i = 0;  i < query.part.index_num; i++) {
                memcpy(bytes + p, &sum[i], ID);
                p += ID;
                memcpy(bytes + p, &count[i], ID);
                p += ID;
                memcpy(bytes + p, &max[i], ID);
                p += ID;
                memcpy(bytes + p, &min[i], ID);
                p += ID;
            } return {bytes, (Size_t)p};
        }

        void Cal() {
            now_slice = GetIndex(iterator->Get(), query.index_part);
            for (int i = 0; i < query.part.index_num; i++) {
                count[i] = 0;
                sum[i]   = 0;
                max[i] = INT32_MIN;
                min[i] = INT32_MAX;
            }
            while (!Null() && query.compare(now_slice, GetIndex(iterator->Get(), query.index_part)) == EQUAL) {
                Slice s = iterator->Get();
                for (int i = 0; i < query.part.index_num; i++) {
                    Id val = *(Id*)GetIndex(s, {{query.part.range[i]}, 1}).begin;
                    count[i]++;
                    sum[i] += val;
                    max[i] = (max[i] >= val) ? max[i] : val;
                    min[i] = (min[i] <= val) ? min[i] : val;
                }
                iterator->Next();
            }
        }

    };

    GroupQuery(Iterable& iterable, Compare compare, const IndexPart& index_part, const IndexPart& part)
            : iterable(iterable), compare(compare), index_part(index_part), part(part) {
        SetIndexPart(index_part);
    }


};


#endif //WALKERDB_GROUP_H
