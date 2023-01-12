//
// Created by Walker on 2022/9/17.
//
#include "../sql/tree/SqlTree.h"
#include "../sql/tree/SqlNameNode.h"
#include "../sql/tree/SqlTableNode.h"
#include "../sql/tree/SqlProjectionNode.h"
#include "../sql/tree/SqlSelectNode.h"

int main() {
    string x = "select name from student";
    string name = "name";
    string student = "student";
    TreeNode columnNameNode = new SqlIdentifierNode(name);
    TreeNode tableNameNode = new SqlIdentifierNode(student);
    TreeNode columnNode = new SqlColumnNameNode(nullptr, columnNameNode);
    TreeNode tableNode = new SqlTableNameNode(tableNameNode);
    TreeNode projectionElement = new SqlColumnNameProjectionElementNode(columnNode);
    TreeNode projections = new SqlProjectionNode(projectionElement, nullptr);
    TreeNode select = new SqlSelectNode(projections, tableNode);

    select->Debug();
    delete select;
    cout << endl;
    cout << "done!" << endl;
    return 0;
}