//
// Created by Walker on 2022/9/16.
//

#ifndef WALKERDB_SQLTABLENODE_H
#define WALKERDB_SQLTABLENODE_H

#include "SqlTree.h"
#include "SqlExpressionNode.h"

/**
 * Table应该有两种，第一种是使用select编写的查询函数
 * 第二种不使用select，只用单table name或join等组成的表达式。
 * 表达式没写，以后再说，现在就只支持单标识符
 */

struct SqlTableNode : SqlExpressionNode {
};

#include "SqlNameNode.h"
struct SqlTableNameNode : SqlTableNode {
    const SqlIdentifierNode* tableName;
    SqlTableNameNode(TreeNode tableName)
    : tableName((SqlIdentifierNode*) tableName) {}
    ~SqlTableNameNode() {
        if (tableName) delete tableName;
    }
    void Debug() const override {
        tableName->Debug();
    }
};

#endif //WALKERDB_SQLTABLENODE_H
