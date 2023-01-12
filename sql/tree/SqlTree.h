//
// Created by Walker on 2022/9/16.
//

#ifndef WALKERDB_SQLTREE_H
#define WALKERDB_SQLTREE_H

#include "../../definition.h"

#include <vector>
struct SqlTreeNode {
public:
    /**
     * 调试语法分析的手段
     */
    virtual void Debug() const {}
    /**
     * 进行语义分析的接口
     */
    virtual void Check() const {}
    /**
     * 生成最终的执行语句的接口
     */
    virtual void Generate() const {}
};

using TreeNode = SqlTreeNode*;
#endif //WALKERDB_SQLTREE_H
