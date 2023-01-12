//
// Created by 董文阔 on 2022/9/18.
//

#ifndef WALKERDB_LAUNCHER_H
#define WALKERDB_LAUNCHER_H


/**
 * 这里是启动器，启动器最基础的任务是检查数据库空间是否存在
 * 即通过扫描数据库文件检查数据库文件信息，确认该文件是Walker DB的数据库文件
 *
 * 如果是，则根据登陆信息，确定是否转送至用户空间。
 * 如果否且文件不为空，则报错。
 * 如果发现文件为空，则需要进行初始化。
 *
 * 初始化内容包括诸如开启初始页，建立系统空间，根据新建立的系统空间完善数据库配置。
 *
 * 然后更加高级的任务包括诸如恢复系统，检查日志，检查时间戳。
 * 对不同程度的不一致状态进行恢复。从而保证事务的一致性。
 */
class Launcher {
    virtual void Launch() {}
    virtual void Initialize() {}
    virtual void Recover() {}
};

#endif //WALKERDB_LAUNCHER_H
