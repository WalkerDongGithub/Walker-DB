//
// Created by 董文阔 on 2021/11/30.
//

#ifndef WSQL_WALKER_VFS_IMPLEMENT_H
#define WSQL_WALKER_VFS_IMPLEMENT_H
#include "walker_vfs.h"


/**
 *
 *
 * 这是一个测试接口，目前主要用于手动实现多态调用，后期应该改成自动多态，能力不足，抱歉。
 * @param chance
 * @return
 */
walker_vfs walker_load_vfs(walker_status chance);
#endif //WSQL_WALKER_VFS_IMPLEMENT_H
