//
// Created by 董文阔 on 2022/7/17.
//
#include "definition.h"

Size_t get_index_table_record_max_num(Size_t index_size) {
//    return (PAGE_SIZE - ID * 5) / (index_size + ID);
    return 6;
}

Size_t get_list_table_record_max_num(Size_t record_size) {
//    return (PAGE_SIZE - ID - ID - ID) / (record_size);
    return 6;
}

CompareResult string_compare(const Slice& a, const Slice& b) {
    for (Int i = 0; i < a.size; i++) {
        if (a.begin[i] == b.begin[i]) continue;
        else return (a.begin[i] < b.begin[i]) ? CompareResult::LESS : CompareResult::LARGE;
    } return CompareResult::EQUAL;
}

Slice GetIndex(const Slice& s, const IndexPart& part) {
    Byte content[MAX_SLICE_SIZE];
    Ptr p = s.begin;
    Size_t size = 0;
    for (Int i = 0; i < part.index_num; i++) {
        memcpy(content + size, p + part.range[i].first, part.range[i].second);
        size += part.range[i].second;
    } return {content, size};
}

IndexPart EmptyIndexPart() {
    IndexPart part;
    part.index_num = 0;
    return part;
}

CompareResult integer_compare(const Slice &a, const Slice &b) {
    long long a_int = a.GetInteger();
    long long b_int = b.GetInteger();
    if (a_int < b_int) return LESS;
    else if (a_int > b_int) return LARGE;
    else return EQUAL;
}
