//
// Created by 董文阔 on 2022/9/18.
//

#ifndef WALKERDB_DATABASE_H
#define WALKERDB_DATABASE_H


/**
 * 数据库系统，实际就是控制中枢，进入数据库系统之后，将进入控制中心。
 * 其内容包括
 * 1. 建立用户空间连接（维护调用各种类型Client线程）
 * 2. 将用户发来的请求拆包送交Parser
 * 3. 根据Parser形成的控制语句列表，确认其信息类型。
 *    如果是数据库控制信息，则送交用户管理，
 *    如果是DML和DQL，则检查用户访问权限后送交事务管理。
 * 4. 对执行结果作出评估，并将结果送交回Client线程。
 * todo: 综上分析，我需要一个能够实现基本输入输出的Client
 * todo: 一个能够解析DDL DML DQL的 Parser
 * todo: 一个能够异步响应用户请求的用户管理 UserManager
 * todo: 一个能够执行DML和DQL的执行引擎 Engine
 *
 */

class DataBase {

};

#endif //WALKERDB_DATABASE_H
