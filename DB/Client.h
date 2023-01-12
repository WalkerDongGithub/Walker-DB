//
// Created by 董文阔 on 2022/9/18.
//

#ifndef WALKERDB_CLIENT_H
#define WALKERDB_CLIENT_H


#include "../definition.h"

/**
 * Client 是一个用户与数据库访问的接口，任何用户都必须通过Client来实现对数据库的访问
 * Client 的主要任务包括管理各类型用户接入，包括远程网络接入，普通控制台接入（Debug）
 * 然后实现用户与数据库交互的功能。
 */

class Client {
public:

    virtual string Input() = 0;
    virtual void Output() = 0;

};

#endif //WALKERDB_CLIENT_H
