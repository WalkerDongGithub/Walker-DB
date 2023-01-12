//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_DISK_H
#define WALKERDB_DISK_H
#include "../../definition.h"
#include "../../page/page.h"
#include "DebugLog.h"
/**
 * 数据库对磁盘的抽象，用于实现跨平台，平台需要提供接口实现。
 * 接口大致只有包括读取某磁盘块，写入某磁盘块内容
 */
class Disk {
public:

    /**
     * 需要提供从磁盘中读取一个磁盘块的方法。
     */
    virtual Status Read(Id i, Page& page) = 0;

    /**
     * 需要提供写入磁盘某一个磁盘块的方法。
     */
    virtual Status Write(Id i, Page& page) = 0;

    /**
     * 需要提供创建一个完全新的磁盘块的方法。
     */
    virtual Status New(Id& i) = 0;

    /**
     * 需要提供一个返回磁盘现在存储信息块数的方法
     */
    virtual Size_t Size() = 0;

};


#endif //WALKERDB_DISK_H
