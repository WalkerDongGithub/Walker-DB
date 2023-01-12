#ifndef WALKERDB_BPLUSTREEINDEXTABLE_H
#define WALKERDB_BPLUSTREEINDEXTABLE_H

#include "file_system/AbstractFileSystem.h"
#include "page_type/b_plus_page.h"
#include "table_type/Table.h"


class BPlusTreeIndexTable : public ToPageIdIndexTable {

    Size_t index_size;
    FileSystem &fs;
    Id root_page;
    Int root_begin;
    Compare compare;

public:

    BPlusTreeIndexTable(FileSystem &fs, Id root_page, Int root_begin, Size_t index_size, Compare compare) :
            fs(fs), index_size(index_size), root_page(root_page), root_begin(root_begin), compare(compare) {
    }

    BPlusTreeIndexTable(const BPlusTreeIndexTable& t) :
            fs(t.fs), index_size(t.index_size), root_page(t.root_page), root_begin(t.root_begin), compare(t.compare) {
    }


    Int Insert(const Slice &key, const Slice &value) override {
        Id to_page_id = *(Id*)value.begin;
        if (RootPage() == NULL_ID) {
            BPlusPage page = BPlusPage::New(NULL_ID, NULL_ID, NULL_ID, NULL_ID, true, fs, index_size);
            RootPage(page.PageId());
        }
        if (Insert(RootPage(), key, to_page_id)) {
            return OK;
        } else {
            return ALREADY_EXISTS;
        }
    }

    Int Remove(const Slice& key) override {
        if (RootPage() == NULL_ID) return false;
        else return Remove(RootPage(), key);
    }

    Slice Query(const Slice& key) override {
        Id result;
        if (RootPage() == NULL_ID) {
            result = NULL_ID;
        } else {
            result = Query(RootPage(), key);
        }
        return {(Ptr)&result, ID};
    }

    FileSystem& FS() override {
        return fs;
    }
    Size_t RecordSize() override {
        return index_size;
    }
    Iterator* GenerateIterator() override {
        return new BPlusTreeIndexTableIterator(*this);
    }


    class BPlusTreeIndexTableIterator : public Iterator {

        BPlusTreeIndexTable& table;
        Id page_id;
        Id index_id;

        Slice queryResult;

    public:
        BPlusTreeIndexTableIterator(BPlusTreeIndexTable& table): table(table) {
            Init();
        }
        void  Init() override {
            if (precise.size != 0) {
                queryResult = table.Query(precise);
            } else {
                page_id = table.First();
                index_id = 0;
            }
        }
        bool  Null() override {
            if (precise.size != 0) {
                return queryResult.begin == NULL;
            } else {
                return page_id == NULL_ID;
            }
        }
        void  Next() override {
            if (precise.size != 0) {
                queryResult.begin = NULL;
            } else {
                index_id++;
                if (index_id == table.Get(page_id).Size()) {
                    page_id = table.Next(page_id);
                    index_id = 0;
                }
            }
        }
        Slice Get() override {
            if (precise.size != 0) {
                return queryResult;
            } else {
                return table.Get(page_id).Index(index_id);
            }
        }
    };

    Id First() {
        if (RootPage() == NULL_ID) return NULL_ID;
        else {
            return First(RootPage());
        }
    }

    Id Next(Id page_id) {
        if (page_id == NULL_ID) return NULL_ID;
        else {
            return Get(page_id).Next();
        }
    }

    Slice Get(Id page_id, Id index_id) {
        return Get(page_id).Index(index_id);
    }

    Size_t Size(Id page_id) {
        return Get(page_id).Size();
    }

    Int Recycle() override {
        return OK;
    }


private:


    Id First(Id page_id) {
        if (Get(page_id).IsLeaf()) return page_id;
        else {
            return First(Get(page_id).Child(0));
        }
    }


    Id Query(Id page_id, const Slice& index) {
        auto v = Get(page_id);

        Int num = v.Size();
        for (Int i = 0; i < num; i++) {
            switch (compare(v.Index(i), index)) {
                case LESS: {
                    continue;
                }
                case EQUAL: {
                    if (!v.IsLeaf()) return Query(v.Child(i), index);
                    else {
                        return v.Child(i);
                    }
                }
                case LARGE: {
                    if (!v.IsLeaf()) return Query(v.Child(i), index);
                    else return NULL_ID;
                }
            }
        } return NULL_ID;
    }

    bool Insert(Id page_id, const Slice& index, Id to_page_id) {

        auto v = Get(page_id);

        Int num = v.Size();
        for (Int i = 0; i < num; i++) {
            switch (compare(v.Index(i), index)) {
                case LESS: {
                    continue;
                }
                case EQUAL: {
                    return false;
                }
                case LARGE: {
                    if (!v.IsLeaf())
                        return Insert(v.Child(i), index, to_page_id);
                    else {
                        v.Insert(i, to_page_id, index);
                        Adjust(page_id);
                        return true;
                    }
                }
            }
        }
        if (!v.IsLeaf())
            return Insert(v.Child(num - 1), index, to_page_id);
        else {
            v.Insert(v.Size(), to_page_id, index);
            Adjust(page_id);
            return true;
        }
    }

    bool Remove(Id page_id, const Slice& index) {
        auto v = Get(page_id);

        Int num = v.Size();
        for (Int i = 0; i < num; i++) {
            switch (compare(v.Index(i), index)) {
                case LESS: {
                    continue;
                }
                case EQUAL: {
                    if (!v.IsLeaf()) return Remove(v.Child(i), index);
                    else {
                        v.Remove(i);
                        Adjust(page_id);
                        return true;
                    }
                }
                case LARGE: {
                    if (!v.IsLeaf()) return Remove(v.Child(i), index);
                    else return false;
                }
            }
        } return false;
    }

    BPlusPage Get(Id page_id) {
        return BPlusPage(
                fs, page_id, index_size
                );
    }

    void Adjust(Id page_id) {
        if (page_id == NULL_ID) return;
        BPlusPage p = Get(page_id);
        Size_t max_num = get_index_table_record_max_num(index_size);
        Int num = p.Size();
        if (num == max_num) { // Full load. split up.

            BPlusPage new_p = BPlusPage::New(
                    p.Parent(), p.ParentIndexId(),
                    p.Next(), p.PageId(),
                    p.IsLeaf(), fs, index_size);

            Int middle = num / 2;
            for (Int i = num - 1; i >= middle; i--) {
                new_p.Insert(0, p.Child(i), p.Index(i));
                p.Remove(i);
            }

            BPlusPage parent = Get(p.Parent());
            if (parent.PageId() == NULL_ID) {
                BPlusPage new_root = BPlusPage::New(
                        NULL_ID, NULL_ID,
                        NULL_ID, NULL_ID,
                        false, fs, index_size);
                new_root.Insert(new_root.Size(), page_id);
                new_root.Insert(new_root.Size(), new_p.PageId());
                RootPage(new_root.PageId());
                return;
            } else {
                parent.Insert(p.ParentIndexId() + 1, new_p.PageId());
                Adjust(parent.PageId());
            }

        } else if (num < max_num / 2) { // lack load. borrow or combine.

            if (p.PageId() != RootPage()) {
                BPlusPage parent = Get(p.Parent());
                if (p.ParentIndexId() > 0 && Get(parent.Child(p.ParentIndexId() - 1)).Size() >
                                                     max_num / 2) {
                    // borrow left.
                    BPlusPage left = Get(parent.Child(p.ParentIndexId() - 1));
                    Id x = left.Size() - 1;
                    p.Insert(0, left.Child(x), left.Index(x));
                    left.Remove(x);

                } else if (p.ParentIndexId() < parent.Size() - 1 &&
                Get(parent.Child(p.ParentIndexId() + 1)).Size() > max_num / 2) {

                    // borrow right.
                    BPlusPage right = Get(parent.Child(p.ParentIndexId() + 1));
                    p.Insert(p.Size(), right.Child(0), right.Index(0));
                    right.Remove(0);

                } else if (p.ParentIndexId() > 0) {

                    // combine left.
                    BPlusPage left = Get(parent.Child(p.ParentIndexId() - 1));
                    while (left.Size() != 0) {
                        p.Insert(0, left.Child(left.Size() - 1), left.Index(left.Size() - 1));
                        left.Remove(left.Size() - 1);
                    }
                    BPlusPage::Delete(left.PageId(), fs, index_size);
                    Adjust(parent.PageId());

                } else if (p.ParentIndexId() < parent.Size() - 1) {

                    // combine right.
                    BPlusPage right = Get(parent.Child(p.ParentIndexId() + 1));
                    while (right.Size() != 0) {
                        p.Insert(p.Size(), right.Child(0), right.Index(0));
                        right.Remove(0);
                    }
                    BPlusPage::Delete(right.PageId(), fs, index_size);
                    Adjust(parent.PageId());

                }

            } else if (num == 1 && !p.IsLeaf()) {

                Id child = p.Child(0);
                BPlusPage::Delete(p.PageId(), fs, index_size);
                RootPage(child);
                return;

            } else if (num == 0 && p.IsLeaf()) {

                BPlusPage::Delete(p.PageId(), fs, index_size);
                RootPage(NULL_ID);
                return;

            }
        }
    }

    Id RootPage() {
        Ptr p = fs.Read(root_page).GetConstData();
        return *(Id*)(p + root_begin);
    }

    void RootPage(Id i) {
        fs.Write(root_page, {(Ptr)&i, ID}, root_begin);
    }

public:
    void Debug() override {
        if (RootPage() == NULL_ID) cout << "none!" << endl;
        else {
            auto v = Get(RootPage());
            v.Debug();
        }
    }

};


#endif //WALKERDB_BPLUSTREEINDEXTABLE_H
