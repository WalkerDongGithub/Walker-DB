//
// Created by Walker on 2022/7/19.
//

#ifndef WALKERDB_ABSTRACTFILESYSTEM_H
#define WALKERDB_ABSTRACTFILESYSTEM_H

#include "../../definition.h"
#include "page.h"
#include "DebugLog.h"

class FileSystem {
public:
    virtual const Page& Read(Id i) = 0;
    virtual Status Write(Id i, const Slice& slice, Int begin) = 0;
    virtual Id New() = 0;
    virtual void Delete(Id id) = 0;
    virtual void DropTable(Id root_page, Id root_begin) {

    }
    virtual Size_t Size() = 0;
    virtual Status WriteAllBack() = 0;

};


#endif //WALKERDB_ABSTRACTFILESYSTEM_H
