//
// Created by 董文阔 on 2022/9/23.
//

#include "DDLEngine.h"
#include "file_system/MemoryFileSystem.h"
static MemoryFileSystem fs;



int main() {
    DDLEngine engine(fs);

    Id root = engine.Mkdir("root");

    vector<ColumnInfo> columnInfos;


    ColumnInfo info;
    info.name = "name";
    info.type = "varchar";
    info.size = 16;
    info.state = 0;
    columnInfos.push_back(info);
    info.name = "id";
    info.type = "int";
    info.size = 4;
    info.state = 1;
    columnInfos.push_back(info);
    info.name = "age";
    info.type = "int";
    info.size = 4;
    info.state = 0;
    columnInfos.push_back(info);
    info.name = "gender";
    info.type = "int";
    info.size = 1;
    info.state = 1;
    columnInfos.push_back(info);

    File root_dir(fs, root);
    Id fcb = engine.Touch(root_dir, "student", columnInfos);

    auto res = engine.List(root_dir);
    for (const auto& x : res) {
        cout << x << "     ";
    } cout << endl;
    File f(fs, fcb);
    map<string, string> item;
    item["name"] = "walker";
    item["id"] = "2019";
    item["age"] = "18";
    item["gender"] = "1";
    f.Insert(item);
    f.AllOut();


    return 0;

}