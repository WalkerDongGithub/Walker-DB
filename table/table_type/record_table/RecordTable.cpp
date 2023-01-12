//
// Created by 董文阔 on 2022/9/22.
//

#include "../Table.h"
#include "QueueRecordTable.h"

RecordTable *
RecordTable::Create(RecordTableType type, Size_t record_size, FileSystem &fs, Id root_page, Id root_begin) {
    switch (type) {
        case QueueRecordTableType:
        default:
            return new QueueRecordTable(
                    root_page, root_begin,
                    fs, record_size
            );
    }
}


