//
// Created by Walker on 2022/7/19.
//

#include "file_system/MemoryFileSystem.h"
#include "DebugLog.h"
#include <string>
#include <random>
#include "all_test.h"
static MemoryFileSystem fs;

static int test_iterator(Iterable& query) {
    Iterator* it = query.GenerateIterator();

    while (!it->Null()) {
        Slice s = it->Get();
        cout << "size : " << s.size << "  ";
        if (s.size % CHAR16 == 0) {
            for (int k = 0; k < s.size; k++) cout << s.begin[k];
        } else if (s.size % (CHAR16 + ID) == 0) {
            for (int i = 0; i < s.size / (CHAR16 + ID); i++) {
                for (int j = 0; j < 16; j++) {
                    cout << s.begin[j + i * (CHAR16 + ID)];
                } cout << " " << *(Id*)(s.begin + CHAR16 + i * (CHAR16 + ID));
            }
        } else if (s.size == 2) {
            cout << s.begin[0] << s.begin[1];
        } else {

            Int p = 0;
            while (p < s.size) {
                cout << *(Id*)(s.begin + p) << " ";
                p += ID;
            }

        }
        cout << endl;
        it->Next();
    } cout << endl;
    delete it;
    return 0;
}

static int test_iterable_index() {

    cout << "test table with index" << endl;
    Id i;
    i = fs.New();
    KVTable table = KVTable(fs, i);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        i = fs.New();
        table.Insert(content.c_str(), i);
        test_iterator(table);
    }
    return 0;

}

static int test_iterable_without_index() {

    cout << "test normal table" << endl;
    Id i;
    i = fs.New();
    RecordTable table = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table.Insert({(Ptr) content.c_str(), CHAR16});
        test_iterator(table);
    }

    return 0;

}

static bool f1(const Slice& s) {
    return s.begin[7] == '6';
}

#include "../Query/Select.h"
static int test_iterable_select_query() {

    cout << "test select query" << endl;
    Id i;
    i = fs.New();
    RecordTable table = RecordTable(
            i,
            0,
            ID,
            fs,
            CHAR16);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table.Insert({(Ptr)content.c_str(), CHAR16});

        SelectQuery query(table, f1);
        test_iterator(query);
    }
    return 0;
}

#include "../Query/Projection.h"
static int test_iterable_projection() {

    cout << "test projection" << endl;
    Id i;
    i = fs.New();
    RecordTable table = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table.Insert({(Ptr)content.c_str(), CHAR16});

        ProjectionQuery query(table, {{{7, 2}}, 1});
        test_iterator(query);
    }
    return 0;
}

#include "../Query/Union.h"
static int test_iterable_union() {

    cout << "test union" << endl;
    Id i;
    i = fs.New();
    RecordTable table = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table.Insert({(Ptr)content.c_str(), CHAR16});

        ProjectionQuery query1(table, {{{7, 2}}, 1});
        ProjectionQuery query2(table, {{{7, 2}}, 1});
        UnionQuery union_query = UnionQuery(query1, query2);
        test_iterator(union_query);
    }
    return 0;
}

#include "../Query/Sort.h"
static int test_iterable_sort() {

    cout << "test sort" << endl;
    Id i;
    i = fs.New();
    RecordTable table = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table.Insert({(Ptr) content.c_str(), CHAR16});
    }

    SortQuery query(table, string_compare, {{{0, CHAR16}}, 1});
    test_iterator(query);
    return 0;
}

#include "../Query/Distinct.h"
static int test_iterable_distinct() {

    cout << "test distinct" << endl;
    Id i;
    i = fs.New();
    RecordTable table = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(0);
    uniform_int_distribution<Id> uid(0, 100);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table.Insert({(Ptr) content.c_str(), CHAR16});
    }

    DistinctQuery query(table, string_compare, {{{0, CHAR16}}, 1});
    test_iterator(query);
    return 0;
}


#include "../Query/Join.h"
static int test_iterable_join() {

    cout << "test join" << endl;
    Id i;
    i = fs.New();
    RecordTable table1 = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    i = fs.New();
    RecordTable table2 = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(time(nullptr));
    uniform_int_distribution<Id> uid(0, 99);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table1.Insert({(Ptr) content.c_str(), CHAR16});
        content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table2.Insert({(Ptr) content.c_str(), CHAR16});
    }

    IndexPart part = {{{7, 2}}, 1};
    JoinQuery query(table1, table2, part, part, string_compare);
    test_iterator(query);
    return 0;
}

#include "../Query/Group.h"
CompareResult int_compare(const Slice& a, const Slice& b) {
    Id x = *(Id*)a.begin, y = *(Id*)b.begin;
    if (x < y) return LESS;
    else if (x == y) return EQUAL;
    else return LARGE;
}

static int test_iterable_group() {

    cout << "test group" << endl;
    Id i;
    i = fs.New();
    RecordTable table1 = RecordTable(
            i,
            0,
            ID,
            fs,
            ID + ID + ID);
    default_random_engine dre(time(nullptr));
    uniform_int_distribution<Id> uid(10, 99);
    for (int j = 0; j < 10; j++) {
        Id x = uid(dre);
        Id y = uid(dre);
        Id z = uid(dre);
        Byte bytes[MAX_SLICE_SIZE];
        memcpy(bytes, &x, ID);
        memcpy(bytes + ID, &y, ID);
        memcpy(bytes + ID + ID, &z, ID);
        table1.Insert({bytes, ID + ID + ID});
    }

    test_iterator(table1);
    IndexPart part = {{{0, ID}, {ID, ID}}, 2};
    IndexPart index_part = {{{ID + ID, ID}}, 1};
    SortQuery s(table1, int_compare, index_part);
    test_iterator(s);
    GroupQuery query(table1, int_compare, index_part, part);
    test_iterator(query);
    return 0;
}

#include "../Query/Intersection.h"
static int test_iterable_intersection() {

    cout << "test intersection" << endl;
    Id i;
    i = fs.New();
    RecordTable table1 = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    i = fs.New();
    RecordTable table2 = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(time(nullptr));
    uniform_int_distribution<Id> uid(0, 99);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table1.Insert({(Ptr) content.c_str(), CHAR16});
        content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table2.Insert({(Ptr) content.c_str(), CHAR16});
    }

    test_iterator(table1);
    test_iterator(table2);
    IntersectionQuery query(table1, table2, string_compare);
    test_iterator(query);
    return 0;

}
#include "../Query/Sub.h"
static int test_iterable_sub() {

    cout << "test sub" << endl;
    Id i;
    i = fs.New();
    RecordTable table1 = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    i = fs.New();
    RecordTable table2 = RecordTable(
            i,
            0,
            ID,
            fs,
            16);
    default_random_engine dre(time(nullptr));
    uniform_int_distribution<Id> uid(10, 99);
    for (int j = 0; j < 10; j++) {
        string content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table1.Insert({(Ptr) content.c_str(), CHAR16});
        content = "walker_" + to_string(uid(dre));
        while (content.size() < CHAR16) content.push_back(' ');
        table2.Insert({(Ptr) content.c_str(), CHAR16});
    }

    SortQuery sort1 = SortQuery(table1, string_compare, {{{0, table1.RecordSize()}}, 1});
    SortQuery sort2 = SortQuery(table2, string_compare, {{{0, table1.RecordSize()}}, 1});

    test_iterator(sort1);
    test_iterator(sort2);
    SubQuery query(table1, table2, string_compare);
    test_iterator(query);
    return 0;

}

int test_iterable() {
    try {
        test_iterable_without_index();
        test_iterable_index();
        test_iterable_select_query();
        test_iterable_projection();
        test_iterable_union();
        test_iterable_sort();
        test_iterable_distinct();
        test_iterable_join();
        test_iterable_group();
        test_iterable_intersection();
        test_iterable_sub();
    } catch (Id e) {
        return 1;
    }
    return 0;
}