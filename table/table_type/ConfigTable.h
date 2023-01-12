//
// Created by 董文阔 on 2022/9/22.
//

#ifndef WALKERDB_CONFIGTABLE_H
#define WALKERDB_CONFIGTABLE_H


#include "WithIndexTable.h"
#include <map>
using std::map;
class ConfigTable {
public:
    virtual Int Open(FileSystem& fs, Id page_id) = 0;
    virtual Slice GetConfig(const string& key) = 0;
    virtual void SetConfig(const string& key, const Slice& value) = 0;
    virtual map<string, Slice> GenerateMap() = 0;
};


#endif //WALKERDB_CONFIGTABLE_H
