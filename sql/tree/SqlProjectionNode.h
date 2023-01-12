//
// Created by Walker on 2022/9/17.
//

#ifndef WALKERDB_SQLPROJECTIONSNODE_H
#define WALKERDB_SQLPROJECTIONSNODE_H

#include "SqlTree.h"
#include "SqlNameNode.h"

/**
 * 投影表达式，即select后边的一堆东西，
 * 用于表征我该如何对表进行投影。
 */

struct SqlProjectionElementNode : SqlTreeNode {};


struct SqlProjectionNode : SqlTreeNode {

    const SqlProjectionElementNode* element;
    const SqlProjectionNode* next;

    SqlProjectionNode(TreeNode element, TreeNode next) :
    element((SqlProjectionElementNode*)element),
    next((SqlProjectionNode*) next) {}

    ~SqlProjectionNode() {
        if (element) delete element;
        if (next) delete next;
    }

    void Debug() const override {
        element->Debug();
        if (next) {
            cout << ", ";
            next->Debug();
        }
    }

};

struct SqlFunctionProjectionElementNode : SqlProjectionElementNode {

    const SqlIdentifierNode* functionName;
    const SqlColumnNameNode* columnName;
    SqlFunctionProjectionElementNode(
            TreeNode functionName,
            TreeNode columnName) :
            functionName((SqlIdentifierNode*)functionName),
            columnName((SqlColumnNameNode*)  columnName) {
    }
    ~SqlFunctionProjectionElementNode() {
        if (functionName) delete functionName;
        if (columnName) delete columnName;
    }
    void Debug() const override {
        functionName->Debug();
        cout << "(";
        columnName->Debug();
        cout << ")";
    }
};

struct SqlColumnNameProjectionElementNode : SqlProjectionElementNode {
    const SqlColumnNameNode* columnName;
    SqlColumnNameProjectionElementNode(TreeNode columnName) :
    columnName((SqlColumnNameNode* ) columnName) {
    }
    ~SqlColumnNameProjectionElementNode() {
        if (columnName) delete columnName;
    }
    void Debug() const override {
        columnName->Debug();
    }
};


#endif //WALKERDB_SQLPROJECTIONSNODE_H
