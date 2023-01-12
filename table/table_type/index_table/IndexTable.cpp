//
// Created by 董文阔 on 2022/9/22.
//
#include "BPlusTreeIndexTable.h"
#include "table_type/Table.h"

IndexTable *IndexTable::Create(IndexTableType type, Size_t index_size, FileSystem &fs, Id root_page, Id root_begin, Compare compare) {
    switch (type) {
        case BPlusTreeIndexTableType:
            return new BPlusTreeIndexTable(
                    fs, root_page,
                    root_begin,
                    index_size,
                    compare
                    );
    } return NULL;
}