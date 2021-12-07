//
// Created by Walker on 2021/11/30.
//

#ifndef WSQL_walker_VFS_H
#define WSQL_walker_VFS_H


/**
 *  仿照 sqlite 的构建方式，第一层是制作一个 vfs 管理系统，sqlite 考虑了跨平台的特性，
 *  我们也暂时使用相似的接口，但是目前只实现 unix 系统下的实现。
 */

#include "../walker_context.h"

/**
 * 根据 sqlite 提供的官方文档，一般 vfs 都实现了 sqlite_vfs 接口用来实现大量的系统调用，我们只实现其中一些功能
 * 以在不影响使用的情况下，尽可能的复现sqlite的体系结构。
 *
 * 接口声明，如无特殊情况，结构体中 walker_ 开头的所有的函数都以第二个参数作为 status 输入。
 * 如果没有特殊的输入参数，则按照默认参数 default 传入，程序也根据默认选择进行运行。
 */

typedef struct walker_vfs            walker_vfs;
typedef struct walker_file           walker_file;
typedef struct walker_file_function  walker_file_function;

struct walker_vfs {

    walker_status (*xOpen)(
            walker_vfs*,
            walker_status  status,
            walker_name    file_name,
            walker_file*   pFile
            );
};

/**
 * walker_file
 * 描述了一个抽象意义的文件
 */

struct walker_file {
    walker_fd     fd;
    walker_uint64   file_size;
    walker_file_function* pFunction;
};

/**
 * walker_file_function
 * walker_file 实现多态的接口
 * 该接口可能会有静态内存实现，动态内存实现，Unix 系统调用实现和 Windows 系统调用实现等
 * 以为不同的使用场景提供不同的服务。
 */
struct walker_file_function {
    walker_status (*xClose)     (walker_file*);
    walker_status (*xSet)       (walker_file*, walker_uint64);
    walker_status (*xAppend)    (walker_file*, walker_uint64);
    walker_status (*xRead)      (walker_file*, walker_ptr, walker_uint64, walker_uint64*);
    walker_status (*xWrite)     (walker_file*, walker_ptr, walker_uint64, walker_uint64*);
    walker_status (*xSize)      (walker_file*);
};


#endif //WSQL_walker_VFS_H
