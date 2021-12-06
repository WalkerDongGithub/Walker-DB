//
// Created by 董文阔 on 2021/11/30.
//

#ifndef WSQL_WALKER_VM_H
#define WSQL_WALKER_VM_H


#include "../walker_context.h"

#include "../vfs/walker_vfs_implement.h"
/**
 * walker_vm
 * 实现了数据库基本的操作功能，包括虚拟文件系统，线程调度。
 *
 * 虚拟文件接口：
 *
 *      上层调用vm时需要使用vm提供的虚拟文件接口，
 *      vm维护一个接口，在上层看来，所有的文件都是可伸缩的，整个文件实际是一个文件夹
 *      上层调用可以申请创建文件，删除文件以及文件读写操作，甚至可以申请创建文件夹。
 *      但是实际上所有的文件操作都是利用同一个文件完成的。
 *      采用的技术是文件的分页管理技术和LRU调度技术。
 *
 *      当虚拟机打开一个.w文件时，就对文件内容进行分页管理。整个文件系统包括
 *      一个配置首页，记录用于维护整个文件系统的基本信息，
 *      包括空表信息、版本信息、以及具有根目录功能。
 *
 *
 */

typedef struct walker_vm            walker_vm;

struct walker_vm {

    walker_vfs       vfs;
    walker_file      file;

};



walker_status walker_vm_write_all(
        walker_file*    file,
        walker_page_id  page_id,
        walker_ptr      buffer,
        walker_uint     size
);
walker_status  walker_vm_init                  (walker_vm*);
walker_status  walker_vm_destroy               (walker_vm*);
walker_page_id walker_vm_new_table            (walker_file*);
#endif //WSQL_WALKER_VM_H
