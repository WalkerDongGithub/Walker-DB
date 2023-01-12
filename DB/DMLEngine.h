//
// Created by 董文阔 on 2022/9/24.
//

#ifndef WALKERDB_DMLENGINE_H
#define WALKERDB_DMLENGINE_H

#include "File.h"

class DMLEngine {
    FileSystem& fs;
public:

    DMLEngine(FileSystem& fs) : fs(fs) {

    }
};

#endif //WALKERDB_DMLENGINE_H
