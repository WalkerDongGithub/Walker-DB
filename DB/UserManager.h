//
// Created by 董文阔 on 2022/9/18.
//

#ifndef WALKERDB_USERMANAGER_H
#define WALKERDB_USERMANAGER_H



/**
 * 用户管理，有两方面任务，一方面，它应能够在并发条件下响应各类用户的控制请求，并设置权限，
 * 对不同的请求作出响应
 *
 * 一方面，他还担负着生成用户事务的完整信息的任务，确认一个事务是一个可执行的，有可能提交的事务。
 */

class UserManager {

};

#endif //WALKERDB_USERMANAGER_H
