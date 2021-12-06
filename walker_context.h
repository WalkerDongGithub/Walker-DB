//
// Created by Walker on 2021/11/30.
//

#ifndef WSQL_walker_CONTEXT_H
#define WSQL_walker_CONTEXT_H

/**
 * 该头文件记录了所有有关 walker sql 的使用过程中使用的常数等信息。
 * 为了方便查看，少数使用频率较低的常数可以被记录其确切的使用位置。
 * 频繁使用的常数可能会列入文档。
 */


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

typedef unsigned long           walker_uint;
typedef long                    walker_int;
typedef long                    walker_page_id;
typedef char*                   walker_string;
typedef const char*             walker_name;
typedef int                     walker_status;
typedef int                     walker_fd;
typedef unsigned char           walker_byte;
typedef walker_byte*            walker_ptr;
typedef const walker_ptr        walker_const_ptr;
typedef sem_t                   walker_semaphore;
typedef unsigned char           walker_bool;

#define walker_true             1
#define walker_false            0

#define walker_null             0   // 代表空指针
#define walker_end             -1   // 代表结尾，用于标识页表指针。
#define walker_ok               1   // 代表一个正确返回。
#define walker_error           -1   // 代表一个错误返回，顺便还能表示debug的错误等级。
#define walker_info             2   // 代表正常等级。
#define walker_warning          3   // 代表警告等级。
#define walker_empty_operation  0   // 代表一个空操作，即还尚未实现的功能。
#define walker_default          0   // 代表一个默认参数。

#define walker_close       1 << 2   // 代表一个页面管理器正在打开某页面
#define walker_open        1 << 1   // 代表一个页面管理器已经关闭页面

#define walker_page_size       1024  // 每一个页面的大小都是一致的，这样在磁盘和内存交互时可以很方便的写入写回。
#define walker_page_id_size    sizeof(walker_int)

#define walker_LRU_size        199


#define walker_allocated       1 << 1

#define max_static_memory           (1 << 10) << 10
#define walker_memory_file      1 << 10 // 代表目标文件实际是一段静态内存。
#define walker_memory_vfs       1 << 1

#define min(a, b)  (a < b) ? a : b


#define walker_error_permission_denied 9




void debug(walker_string info, walker_status level);




#endif //WSQL_walker_CONTEXT_H
