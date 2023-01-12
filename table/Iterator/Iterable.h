//
// Created by Walker on 2022/7/22.
//

#ifndef WALKERDB_ITERABLE_H
#define WALKERDB_ITERABLE_H

#include "../../definition.h"
#include "file_system/AbstractFileSystem.h"

class Iterator;
class Iterable {
private:
    IndexPart indexPart;
public:
    Iterable() : indexPart(EmptyIndexPart()) {}

    void SetIndexPart(const IndexPart& part) {
        indexPart = part;
    }
    const IndexPart& GetIndexPart() const {
        return indexPart;
    }
    bool Sorted() const {
        return indexPart.index_num != 0;
    }
    virtual FileSystem& FS() = 0;
    virtual Size_t RecordSize() = 0;
    virtual Iterator* GenerateIterator() = 0;
};
/**
 * 正确用法是：
 * Iterator* it = table.GenerateIterator(); // init();
 * while (!it->Null()) {
 *      ----it->Get();
 *      it->Next();
 * }
 *
 */
class Iterator {
protected:
    Slice precise;
public:
    virtual ~Iterator() = default;
    virtual void  Init() = 0;
    virtual bool  Null() = 0;
    virtual void  Next() = 0;
    virtual Slice Get() = 0;
    void SetPrecise(const Slice& precise) {
        this->precise = precise;
    }
};



#include "MemoryManager.h"


#endif //WALKERDB_ITERABLE_H
