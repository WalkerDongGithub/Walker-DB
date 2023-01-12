//
// Created by Walker on 2022/7/19.
//

#ifndef WALKERDB_WithIndexTable_H
#define WALKERDB_WithIndexTable_H

#include "table_type/Table.h"
#include "Iterator/Iterable.h"
/**
 *  带索引表，一般包括一个无索引表和一个索引表相结合组成带索引表
 *
 *  所以带索引表不是表，而是至少两个表的集合，是一个更高级别的操作结构
 *
 *  从用户看来，带索引表就相当于一个字典结构，可以通过索引快速查询到一个记录项
 *  带索引表为更上级提供操作接口，功能包括
 *  查询表项    查询使用索引进行查询
 *  插入表项    插入时不需要索引
 *  删除表项    删除使用索引进行查询，然后进行删除
 *  更新表项    更新不需要索引
 *  删除该表    将整个表删除
 *
 *
 *  这里对索引表有一个要求，就是索引表键必须映射到页号上，而不是什么其他的东西
 *
 *
 */
class WithIndexTable : public ReadableAndWritable {

    ToPageIdIndexTable* indexTable;
    RecordTable* recordTable;
    IndexPart indexPart;
    Size_t record_size;

public:

    WithIndexTable(
            IndexTableType indexTableType,
            RecordTableType recordTableType,
            FileSystem& fs,
            Size_t recordSize,
            const IndexPart& indexPart,
            Id rootPage, Id root,
            Compare compare
            ): indexPart(indexPart), record_size(recordSize) {
        indexTable = (ToPageIdIndexTable*)IndexTable::Create(
                indexTableType,
                indexPart.Size(),
                fs,
                rootPage,
                root,
                compare);
        recordTable = RecordTable::Create(
                recordTableType,
                recordSize,
                fs,
                rootPage,
                root + ID);
    }

    ~WithIndexTable() {
        delete recordTable;
        delete indexTable;
    }


    /**
     * 实现查询返回
     * @param key   查询键
     * @return      查询值
     */
    Slice Query(const Slice& key) {
        Id page_id = indexTable->QueryPageId(key);
        if (page_id != NULL_ID) {
            Id record_id = recordTable->GetByIndex(page_id, key, indexPart);
            return recordTable->GetById(page_id, record_id);
        } else {
            return {};
        }
    }

    /**
     * 实现键值插入
     * @param value 插入值
     * @return      插入位置或错误信息
     */
    Id Insert(const Slice& value) override {
        Id loc = recordTable->Insert(value);
        if (indexTable->InsertPageId(Index(value), loc) == ALREADY_EXISTS) {
            Id record_id = recordTable->GetByIndex(loc, Index(value), indexPart);
            if (record_id == NULL_ID) return ERROR_NULL_POINTER;
            recordTable->Remove({NULL, 0}, loc, record_id);
            return ALREADY_EXISTS;
        } return loc;
    }

    /**
     * 实现键值对删除
     * @param key   删除键
     * @return      删除状态
     */
    Int Remove(const Slice& key, Id page_id = NULL_ID, Id record_id = NULL_ID) override {
        Id loc = indexTable->QueryPageId(key);
        if (loc != NULL_ID) {
            record_id = recordTable->GetByIndex(loc, key, indexPart);
            if (record_id == NULL_ID) return ERROR_NULL_POINTER;
            recordTable->Remove({NULL, 0}, loc, record_id);
            indexTable->Remove(key);
        } return OK;
    }

    /**
     * 实现键值对更新
     * @param key   更新键
     * @param value 更新值
     * @return      更新状态
     */
    Int Update(const Slice& value, Id page_id = NULL_ID, Id record_id = NULL_ID) override {
        Id loc = indexTable->QueryPageId(Index(value));
        if (loc != NULL_ID) {
            record_id = recordTable->GetByIndex(loc, Index(value), indexPart);
            if (record_id == NULL_ID) return ERROR_NULL_POINTER;
            recordTable->Update(value, loc, record_id);
        } return OK;
    }


    /**
     * 删除该表的操作
     * @return      删除状态
     */
    Int Drop() {
        return OK;
    }



    /**
     * Iterator Behaviour.
     *
     */
     Iterator * GenerateIterator() override {
         return new WithIndexTableIterator(*this);
     }
     Size_t RecordSize() override {
         return record_size;
     }
     FileSystem & FS() override {
         return recordTable->FS();
     }

    class WithIndexTableIterator : public Iterator {

        WithIndexTable& table;
        Iterator* indexTableIterator;

    public:

        WithIndexTableIterator(WithIndexTable& table): table(table) {
            Init();
        }

        void Init() override {
            indexTableIterator = table.indexTable->GenerateIterator();
        }

        void Next() override {
            indexTableIterator->Next();
        }
        bool Null() override {
            return indexTableIterator->Null();
        }

        Slice Get() override {
            Slice key = indexTableIterator->Get();
            Id loc = *(Id*)table.indexTable->Query(key).begin;
            Id id = table.recordTable->GetByIndex(loc, key, table.indexPart);
            return table.recordTable->GetById(loc, id);
        }

    };



     void Debug() {
         Iterator* iterator = GenerateIterator();
         while (!iterator->Null()) {
             Id x = *(Id*)iterator->Get().begin;
             cout << x << " ";
             iterator->Next();
         } cout << endl;
         delete iterator;
     }

private:

    Slice Index(const Slice& record) {
        return GetIndex(record, indexPart);
    }

};


#endif //WALKERDB_WithIndexTable_H
