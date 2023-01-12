//
// Created by Walker on 2022/9/16.
//

#ifndef WALKERDB_SQLCOMMANDNODE_H
#define WALKERDB_SQLCOMMANDNODE_H

#include "SqlTree.h"
/**
 * 所有表达式的集合，用于区分DDL，DML，DQL的重要节点
 */
struct SqlExpressionNode : SqlTreeNode {
};

#endif //WALKERDB_SQLCOMMANDNODE_H
