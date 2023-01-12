//
// Created by Walker on 2022/9/16.
//

#ifndef WALKERDB_SQLSELECTSUBNODE_H
#define WALKERDB_SQLSELECTSUBNODE_H

#include "SqlTree.h"
#include "SqlTableNode.h"
#include "SqlProjectionNode.h"

/**
 * Select 语句
 */
struct SqlSelectNode : SqlTableNode {
    const SqlProjectionNode* projection;
    const SqlTableNode* table;

    SqlSelectNode(TreeNode projection, TreeNode table):
    projection((SqlProjectionNode*)projection),
    table((SqlTableNode*) table) {}

    ~SqlSelectNode() {
        if (projection) delete projection;
        if (table) delete table;
    }
    void Debug() const override {
        cout << " select ";
        projection->Debug();
        cout << " from ";
        table->Debug();
    }

};


#endif //WALKERDB_SQLSELECTSUBNODE_H
