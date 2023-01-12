//
// Created by Walker on 2022/7/19.
//

#ifndef WALKERDB_KVTABLE_H
#define WALKERDB_KVTABLE_H

#include "ConfigTable.h"
#include "WithIndexTable.h"


class KVTable : public WithIndexTable {

public:

    KVTable(FileSystem& fs, Id root_page, Id root_begin):
            WithIndexTable(
                    BPlusTreeIndexTableType,
                    QueueRecordTableType,
                    fs,
                    CHAR64,
                    {{{0, CHAR32}}, 1},
                    root_page,
                    root_begin,
                    string_compare) {

    }

    void PrintOut() {
        Iterator* it = this->GenerateIterator();
        while (!it->Null()) {
            Slice s = it->Get();
            string key;
            for (int i = 0; i < CHAR32 && s.begin[i] != 0; i++) {
                key.push_back(s.begin[i]);
            } cout << key << " : ";
            string value;
            for (int i = CHAR64 - CHAR32; i < CHAR64 && s.begin[i] != 0; i++) {
                value.push_back(s.begin[i]);
            } cout << value << endl;
            it->Next();
        }
    }

};


#endif //WALKERDB_KVTABLE_H
