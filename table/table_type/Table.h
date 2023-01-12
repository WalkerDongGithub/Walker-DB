//
// Created by 董文阔 on 2022/9/22.
//

#ifndef WALKERDB_TABLE_H
#define WALKERDB_TABLE_H

#include "../definition.h"
#include "file_system/AbstractFileSystem.h"
#include "page_type/list_page.h"
#include "Iterator/Iterable.h"
/**
 * Recyclable，
 * 需要一个回收操作，负责将该表彻底删除。
 */
class Recyclable {
public:

    /**
     * 将整个表删除
     * @return       返回删除状态
     */
    virtual Int Recycle() {
        return OK;
    }

};


class Updatable {

public:
    /**
     * 插入一条记录，然后返回插入的位置对应的页号，这里具体的插入方式根据不同的表有不同的行为
     * @param record      记录内容
     * @return            返回插入的页号，如果插入失败，则返回错误标识
     */
    virtual Id Insert(const Slice& value) = 0;

    /**
     * 更新目标内容，只能通过下标来精确定位进行更新
     * @param page_id       目标页号
     * @param record_id     目标下标
     * @return              删除状态
     */
    virtual Int Update(const Slice& value, Id page_id = NULL_ID, Id record_id = NULL_ID) = 0;

    /**
     * 删除目标内容，通过下标来精确定位进行删除，
     * @param key           利用索引进行定位则使用key
     * @param page_id       不利用索引则使用目标页号
     * @param record_id     和目标下标
     * @return              删除状态
     */
    virtual Int Remove(const Slice& key, Id page_id = NULL_ID, Id record_id = NULL_ID) = 0;


};

class ReadableAndWritable : public Updatable, public Iterable {};

enum IndexTableType {
    BPlusTreeIndexTableType
};
enum RecordTableType {
    QueueRecordTableType
};


/**
 * 索引表，专门用来实现索引操作的表，这里的索引操作包括查询和删除，
 * todo：这里需要一个精确查询的接口，但是没有给出，之后记得补
 */
class IndexTable : public Recyclable, public Iterable {
public:

    /**
     * 插入一条表项，该表项也许是一条记录项，也许是一条索引，依据具体的表类型决定
     * @param key      插入的键
     * @param value    插入的值
     * @return         返回插入的状态，也许会发生重复等
     */
    virtual Int Insert(const Slice& key, const Slice& value) = 0;

    /**
     * 从索引表中查询某一条索引，然后返回查询结果
     * @param key       被查询的目标索引
     * @return          返回查询结果，即对应的值
     */
    virtual Slice Query(const Slice& key) = 0;

    /**
     * 通过索引直接删除一条索引，以及返回删除对应的表项的查询结果
     * @param key       被删除的目标索引
     * @return          返回删除状态
     */
    virtual Int Remove(const Slice& key) = 0;



    virtual void Debug() = 0;

    /**
     * 创建一个 Index Recyclable 操作器
     * @param type          索引类型
     * @param index_size    索引尺寸
     * @param fs            所依赖的文件系统
     * @param root_page     根页号所在页
     * @param root_begin    根页号所在偏移
     * @return
     */
    static IndexTable* Create(
            IndexTableType type,
            Size_t index_size,
            FileSystem& fs,
            Id root_page,
            Id root_begin,
            Compare compare);
};


class ToPageIdIndexTable : public IndexTable {
public:
    Int InsertPageId(const Slice& key, Id value) {
        return Insert(key, Slice((Ptr)&value, ID));
    }
    Id QueryPageId(const Slice& key) {
        return *(Id*)Query(key).begin;
    }
};


/**
 * Record Recyclable
 * 记录表，即无索引表，对于无索引表，
 * 对其操作的主要接口：
 * 一是通过页号和下标直接来作为索引进行操作，
 * 二是通过扫描某页的内容，并通过索引方式来进行查找
 *
 * 这里的记录表目前只实现了队列插入的定长记录表，
 * 这样设计可以将其扩展为一个可变长度的变长记录表
 *
 */
class RecordTable : public Recyclable, public ReadableAndWritable {
public:


    /**
     * 直接通过页号+下标来进行精确定位，返回记录数据
     * @param page_id           目标页号
     * @param record_id         目标下标
     * @return                  返回的记录结果
     */
    virtual Slice GetById(Id page_id, Id record_id) = 0;

    /**
     * 通过使用查找键+索引部分来进行搜索，返回其下标
     * @param page_id           目标页号
     * @param key               查找键
     * @param part              查找索引部分
     * @return                  返回对应的下标
     */
    virtual Id    GetByIndex(Id page_id, const Slice& key, const IndexPart& part) = 0;


    virtual bool Empty() = 0;


    virtual void Debug() = 0;

    /**
     * 创建一个 Record Recyclable 操作器
     * @param type          表类型
     * @param record_size   记录长度
     * @param fs            所依赖的文件系统
     * @param root_page     根页号所在页
     * @param root_begin    根页号所在偏移
     * @return              返回所生成的记录表操作器
     */
    static RecordTable* Create(
            RecordTableType type,
            Size_t record_size,
            FileSystem& fs,
            Id root_page,
            Id root_begin);

};





#endif //WALKERDB_TABLE_H
