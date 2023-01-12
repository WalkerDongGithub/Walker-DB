//
// Created by 董文阔 on 2022/9/23.
//

#ifndef WALKERDB_CONSOLE_H
#define WALKERDB_CONSOLE_H
#include "DDLEngine.h"
#include <iostream>
#include "DQLEngine.h"
using namespace std;
class Console {
    DDLEngine& ddlEngine;
    DQLEngine& dqlEngine;
    vector<Id> path;
    File* currentDir;
public:
    Console(
            DDLEngine& ddlEngine,
            DQLEngine& dqlEngine):
            ddlEngine(ddlEngine),
            dqlEngine(dqlEngine) {
        if (ddlEngine.FS().Size() == 0) {
            path.push_back(ddlEngine.Mkdir("root"));
            currentDir = new File(ddlEngine.FS(), path[0]);
        } else {
            path.push_back(0);
            currentDir = new File(ddlEngine.FS(), path[0]);
        }
    }
    ~Console() {
        delete currentDir;
    }

    string readNext(Id& p, char buf[]) {
        string cmd;
        for (; p < BUFFER_SIZE && buf[p] != ' ' && buf[p] != 0; p++) {
            cmd.push_back(buf[p]);
        } p++;
        return cmd;
    }
    void Run() {
        while (true) {
            cout << ">> ";
            char buf[BUFFER_SIZE];
            cin.getline(buf, BUFFER_SIZE);
            string cmd;
            int p = 0;
            cmd = readNext(p, buf);
            if (cmd == "ls") {
                auto arr = ddlEngine.List(currentDir);
                for (const auto& x : arr) {
                    cout << x << "   ";
                } cout << endl;
            } else if (cmd == "cd") {
                string parameter = readNext(p, buf);
                if (parameter == "..") {
                    delete currentDir;
                    path.pop_back();
                    currentDir = new File(ddlEngine.FS(), path.back());
                } else {
                    Id newDirId = ddlEngine.Cd(parameter, currentDir);
                    path.push_back(newDirId);
                    delete currentDir;
                    currentDir = new File(ddlEngine.FS(), newDirId);
                }
            } else if (cmd == "mkdir") {
                string parameter = readNext(p, buf);
                ddlEngine.Mkdir(parameter, currentDir);
            } else if (cmd == "create_table_test") {
                CreateTableTest();
            } else if (cmd == "select_table_test") {
                SelectTableTest();
            } else if (cmd == "pwd") {
                auto v = ddlEngine.Pwd(path);
                for (const auto& x : v) cout << "/" << x;
                cout << endl;
            } else if (cmd == "cat") {
                string parameter = readNext(p, buf);
                auto v = ddlEngine.Cat(parameter, currentDir);
                for (const auto& x : v) {
                    cout << "{";
                    for (const auto& y : x) {
                        cout << "\"" << y.first << "\":\"" << y.second << "\",";
                    } cout << "}" << endl;
                }
            }
        }

    }



    void CreateTableTest() {
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
        info.state = 0;
        columnInfos.push_back(info);
        ddlEngine.Touch("student", columnInfos, currentDir);
        InsertTableTest();
    }

    void InsertTableTest() {
        File f(ddlEngine.FS(), stoi(currentDir->GetConfig("file_student")));
        map<string, string> item;
        item["name"] = "walker";
        item["id"] = "2019";
        item["age"] = "18";
        item["gender"] = "1";
        f.Insert(item);

        item["name"] = "girl";
        item["id"] = "2020";
        item["age"] = "18";
        item["gender"] = "0";
        f.Insert(item);

        item["name"] = "another_girl";
        item["id"] = "2021";
        item["age"] = "20";
        item["gender"] = "0";
        f.Insert(item);

        item["name"] = "whatever_boy";
        item["id"] = "2022";
        item["age"] = "20";
        item["gender"] = "1";
        f.Insert(item);

        item["name"] = "too_young_girl";
        item["id"] = "2023";
        item["age"] = "17";
        item["gender"] = "0";
        f.Insert(item);
    }


    void SelectTableTest() {
//        CreateTableTest();
        // select id, name
        // from student
        // where age >= 18 and gender = 0
        // order by name;
        Id student_file_id = ddlEngine.Cd("student", currentDir);
        File* student_file = new File(ddlEngine.FS(), student_file_id);
        auto s = dqlEngine.GetFromFile(student_file);

        vector<DQLEngine::Condition> conditions;
        DQLEngine::Condition ca({"age", ">=", "18"});
        DQLEngine::Condition cb({"gender", "=", "0"});
        Filter* fa = ca.GetFilter(s.top());
        Filter* fb = cb.GetFilter(s.top());
        Filter* fc = new DQLEngine::AndFilter(fa, fb);
        dqlEngine.Select(fc, s);
        dqlEngine.Sort("name", s);
        dqlEngine.Projection({"name", "age"}, s);
        auto res = dqlEngine.Return(s);

        cout << "select name, age from student where age >= 18 and gender = 1 order by name" << endl;
        for (const auto& x : res) {
            cout << "{";
            for (const auto& y : x) {
                cout << "\"" << y.first << "\":\"" << y.second << "\",";
            } cout << "}" << endl;
        }



    }

};
#endif //WALKERDB_CONSOLE_H
