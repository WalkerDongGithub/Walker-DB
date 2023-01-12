//
// Created by 董文阔 on 2022/9/23.
//

#ifndef WALKERDB_DDLENGINE_H
#define WALKERDB_DDLENGINE_H


#include "File.h"
#include <vector>
#include <sstream>
using std::stringstream;
using std::vector;
/**
 * create table student(name varchar(64), id int primary key);
 */

struct ColumnInfo {
    string name;
    string type;
    Size_t size;


    /**
     * 00000000
     * 00000001 -> 索引位
     * 00000010 -> 非空位
     * 00000100 -> 存在约束位
     */
    unsigned char state;
};
class DDLEngine {

    FileSystem& fs;

public:
    DDLEngine(FileSystem& fs) : fs(fs) {}

    FileSystem& FS() {
        return fs;
    }
    vector<string> List(File* dir) {
        vector<string> res;
        dir->GetConfig("file_", res);
        return res;
    }

    vector<string> Pwd(const vector<Id>& path) {
        vector<string> res;
        for (const auto& x : path) {
            File file(fs, x);
            res.push_back(file.GetConfig("name"));
        } return res;
    }

    Id Cd(const string& name, File* dir) {
        return stoi(dir->GetConfig("file_" + name));
    }

    Id Mkdir(const string& name, File* dir = nullptr) {
        Id fcb = fs.New();
        KVTable table = KVTable(fs, fcb, FileConfigTableBegin);
        table.Insert(File::GenerateRecord("name", name));
        table.Insert(File::GenerateRecord("type", "directory"));
        if (dir) dir->SetConfig("file_" + name, to_string(fcb));
        return fcb;
    }

    Int Touch(
            const string& name,
            const vector<ColumnInfo>& columns,
            File* dir
            ) {
        Id fcb = fs.New();
        KVTable table = KVTable(fs, fcb, FileConfigTableBegin);
        table.Insert(File::GenerateRecord("name", name));
        table.Insert(File::GenerateRecord("column_num", to_string(columns.size())));
        table.Insert(File::GenerateRecord("record_table_type", "queue"));
        bool with_index = false;
        for (const auto& x : columns) {
            if ((x.state & 1) != 0) {
                table.Insert(File::GenerateRecord("type", "with_index"));
                table.Insert(File::GenerateRecord("index_table_type", "b_plus"));
                with_index = true;
                break;
            }
        }
        if (!with_index) table.Insert(File::GenerateRecord("type", "no_index"));

        int j = 0;
        int begin = 0;
        for (int i = 0; i < columns.size(); i++) {
            table.Insert(File::GenerateRecord(columns[i].name + "_", to_string(i)));
            table.Insert(File::GenerateRecord("column_" + to_string(i) + "_name", columns[i].name));
            table.Insert(File::GenerateRecord("column_" + to_string(i) + "_type", columns[i].type));
            table.Insert(File::GenerateRecord("column_" + to_string(i) + "_size", to_string(columns[i].size)));
            table.Insert(File::GenerateRecord("column_" + to_string(i) + "_state", to_string(columns[i].state)));
            table.Insert(File::GenerateRecord("column_" + to_string(i) + "_begin", to_string(begin)));
            if ((columns[i].state & 1) != 0) {
                table.Insert(File::GenerateRecord("index_part_range_" + to_string(j) + "_begin", to_string(begin)));
                table.Insert(File::GenerateRecord("index_part_range_" + to_string(j) + "_size", to_string(columns[i].size)));
                j++;
            }
            begin += columns[i].size;

        }
        table.Insert(File::GenerateRecord("record_size", to_string(begin)));
        if (with_index) table.Insert(File::GenerateRecord("index_part_size", to_string(j)));
//        File(fs, fcb).CheckConfig();
        cout << endl;
        dir->SetConfig("file_" + name, to_string(fcb));
//        dir->CheckConfig();
        return fcb;
    }


    vector<map<string, string>> Cat(const string& file_name, File* dir) {

        Id target_fcb = stoi(dir->GetConfig("file_" + file_name));
        File file(fs, target_fcb);

        Iterator* it = file.GetIterable()->GenerateIterator();
        vector<map<string, string>> output;
        Size_t column_num = stoi(file.GetConfig("column_num"));
        while (!it->Null()) {
            map<string, string> item;
            Slice s = it->Get();
            for (int i = 0; i < column_num; i++) {
                string name = file.GetConfig("column_" + to_string(i) + "_name");
                string type = file.GetConfig("column_" + to_string(i) + "_type");
                string begin = file.GetConfig("column_" + to_string(i) + "_begin");
                string size = file.GetConfig("column_" + to_string(i) + "_size");
                string res;
                if (type == "int") {
                    Byte content[MAX_SLICE_SIZE] = {};
                    memcpy(content, s.begin + stoi(begin), stoi(size));
                    res = to_string(*(Int*)content);
                } else if (type == "varchar") {
                    string out;
                    for (int k = 0; k < stoi(size) && s.begin[stoi(begin) + k]; k++) {
                        out.push_back(s.begin[stoi(begin) + k]);
                    }
                    res = out;
                }
                item[name] = res;
            }

            output.push_back(item);

            it->Next();
        }
        delete it;
        return output;
        // delete it;
    }



};

#endif //WALKERDB_DDLENGINE_H
