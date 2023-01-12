//
// Created by Walker on 2022/9/17.
//

#ifndef WALKERDB_SQLNAMENODE_H
#define WALKERDB_SQLNAMENODE_H

#include "SqlTree.h"

/**
 * 各类简单名字的节点
 */

struct SqlIdentifierNode : SqlTreeNode {
private:
    string content;
public:
    SqlIdentifierNode(string content) : content(move(content)) {}
    const string& Content() { return content; }

    void Debug() const override {
        cout << content;
    }
};

struct SqlColumnNameNode : SqlTreeNode {

    const SqlIdentifierNode* tableIdentifier;
    const SqlIdentifierNode* columnIdentifier;

    SqlColumnNameNode(TreeNode table, TreeNode column) :
            tableIdentifier( (SqlIdentifierNode*)  table),
            columnIdentifier((SqlIdentifierNode*) column) {
    }

    ~SqlColumnNameNode() {
        if (tableIdentifier) delete tableIdentifier;
        if (columnIdentifier) delete columnIdentifier;
    }
    void Debug() const override {
        if (tableIdentifier == nullptr) {
            columnIdentifier->Debug();
        } else {
            tableIdentifier->Debug();
            cout << ".";
            columnIdentifier->Debug();
        }
    }
};

#endif //WALKERDB_SQLNAMENODE_H
