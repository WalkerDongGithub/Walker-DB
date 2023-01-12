//
// Created by 董文阔 on 2022/9/22.
//

#ifndef WALKERDB_FILE_H
#define WALKERDB_FILE_H

#include "../definition.h"
#include "table_type/WithIndexTable.h"
#include "table_type/Table.h"
#include "table_type/KVTable.h"
#include <memory>
#include <vector>
using std::vector;


struct String64 : public Slice {

    String64(const string& s): Slice((Ptr)s.c_str(), s.size()) {}
    String64(const Slice& s) : Slice(s.begin, s.size) {}

    String64& Combine(const String64& str) {
        memcpy(this->content + this->size, str.content, str.size);
        this->size += str.size;
        return *this;
    }

    bool operator == (const String64& s) const {
        return s.size == this->size &&
        memcmp(this->content, s.content, s.size) == 0;
    }

    string to_string() {
        string s;
        for (int i = 0; i < size && content[i] != 0; i++) {
            s.push_back(this->content[i]);
        } return s;
    }

    void align(int to_size) {
        if (size > to_size) size = to_size;
        while (size < to_size) {
            content[size++] = 0;
        }
    }
};

const Id FileConfigTableBegin = 0;
const Id RecordTableBegin = FileConfigTableBegin + 3 * ID;
class File {
    FileSystem& fs;
    Id fcb_id;

    KVTable fileConfigTable;
    ReadableAndWritable* table = NULL;
public:

    File(FileSystem& fs, Id fcb_id):
    fs(fs), fcb_id(fcb_id),
    fileConfigTable(fs, fcb_id, FileConfigTableBegin)  {
        string type = GetConfig("type");
        if (type == "with_index") {

            table = new WithIndexTable(
                    GetIndexTableType(),
                    GetRecordTableType(),
                    fs,
                    GetRecordSize(),
                    GetIndexPart(),
                    fcb_id,
                    RecordTableBegin,
                    GetComparator());

        } else if (type == "no_index") {

            table = RecordTable::Create(
                    GetRecordTableType(),
                    GetRecordSize(),
                    fs,
                    fcb_id,
                    RecordTableBegin + ID
                    );

        } else if (type == "config") {
            table = NULL;
        } else if (type == "directory") {
            table = NULL;
        } else {
            throw 1;
        }
    }

    File(const File& file) = delete;

    ~File() {
        if (table) delete table;
    }

    void SetConfig(const string& key, const string& value) {
        String64 record = GenerateRecord(key, value);
        Slice query = fileConfigTable.Query(record);
        if (query.size != 0) {
            fileConfigTable.Update(record);
        } else {
            fileConfigTable.Insert(record);
        }
    }

    string GetConfig(const string& key) {
        String64 key_and_value = fileConfigTable.Query(GenerateKey(key));
        Slice value = {key_and_value.begin + CHAR32, CHAR64 - CHAR32};
        return String64(value).to_string();
    }

    void GetConfig(const string& prefix, vector<string>& res) {
        Iterator* it = fileConfigTable.GenerateIterator();
        while (!it->Null()) {
            String64 s = it->Get();
            string key;
            for (int i = 0; i < CHAR32 && s.begin[i]; i++) key.push_back(s.begin[i]);
            string value;
            if (prefix == key.substr(0, prefix.size())) {
                res.push_back(key.substr(prefix.size(), key.size() - prefix.size()));
            }
            it->Next();
        }
    }

    void CheckConfig() {
        fileConfigTable.PrintOut();
    }

    static String64 GenerateKey(const string& key) {
        String64 res = key;
        res.align(CHAR32);
        return res;
    }

    static String64 GenerateValue(const string& value) {
        String64 res = value;
        res.align(CHAR64 - CHAR32);
        return res;
    }

    static String64 GenerateRecord(const string& key, const string& value) {
        String64 res = key;
        res.align(CHAR32);
        res.Combine(value);
        res.align(CHAR64);
        return res;
    }

    void Insert(const map<string, string>& item) {
        Byte content[MAX_SLICE_SIZE];
        memset(content, 0xff, MAX_SLICE_SIZE);

        for (const auto& x : item) {

            string key = x.first;

            string i = GetConfig(key + "_");
            int size = stoi(GetConfig("column_" + i + "_size"));
            int begin = stoi(GetConfig("column_" + i + "_begin"));
            string type = GetConfig("column_" + i + "_type");
            unsigned char state = stoi(GetConfig("column_" + i + "_state"));

            string value = x.second;
            if (type == "int") {
                Int v = stoi(value);
                memcpy(content + begin, (Ptr)&v, size);
            } else if (type == "varchar") {
                while (value.size() < size) value.push_back(0);
                memcpy(content + begin, value.data(), size);
            } else {

            }

        }

        Size_t record_size = GetRecordSize();
        table->Insert(Slice(content, record_size));

    }

    Iterable* GetIterable() {
        return table;
    }

    IndexTableType GetIndexTableType() {
        string value = GetConfig("index_table_type");
        if (value == "b_plus") {
            return BPlusTreeIndexTableType;
        } else {
            return BPlusTreeIndexTableType;
        }
    }

    RecordTableType GetRecordTableType() {
        string value = GetConfig("record_table_type");
        if (value == "queue") {
            return QueueRecordTableType;
        } else {
            return QueueRecordTableType;
        }
    }

    Size_t GetRecordSize() {
        string value = GetConfig("record_size");
        return stoi(value);
    }

    IndexPart GetIndexPart() {
        string value = GetConfig("index_part_size");
        IndexPart part;
        part.index_num = stoi(value);
        for (int i = 0; i < part.index_num; i++) {
            string begin = GetConfig("index_part_range_" + to_string(i) + "_begin");
            string size = GetConfig("index_part_range_" + to_string(i) + "_size");
            part.range[i] = {stoi(begin), stoi(size)};
        }
        return part;
    }

    Compare GetComparator() {
        string value = GetConfig("comparator");
        if (value == string("varchar")) {
            return string_compare;
        } else {
            return integer_compare;
        }
    }



    void CheckForConstraint(int column_id, const string& value, unsigned char state) {}

};



#endif //WALKERDB_FILE_H
