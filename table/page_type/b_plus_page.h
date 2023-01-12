//
// Created by 董文阔 on 2022/7/17.
//

#ifndef WALKERDB_B_PLUS_PAGE_H
#define WALKERDB_B_PLUS_PAGE_H

#include "page.h"
#include "file_system/AbstractFileSystem.h"



class BPlusPage {

    Id page_id;
    FileSystem &fs;
    Size_t index_size;


public:

    BPlusPage(FileSystem &fs, Id page_id, Size_t index_size)
            : page_id(page_id), fs(fs), index_size(index_size) {
    }

public:
    Id Child(Id index_id) {
        Ptr data = fs.Read(page_id).GetConstData();
        return *(Id * )(data + Offset(index_id));
    }

    Slice Index(Id index_id) {
        Ptr data = fs.Read(page_id).GetConstData();
        return {
                data + Offset(index_id) + ID,
                index_size
        };
    }

    Id PageId() {
        return page_id;
    }

private:

    void Child(Id index_id, Id child) {
        fs.Write(page_id, {(Ptr) & child, ID}, Offset(index_id));
        if (!IsLeaf()) {
            Get(child, fs, index_size).Parent(PageId());
            Get(child, fs, index_size).ParentIndexId(index_id);
        }
    }

    void Index(Id index_id, Slice new_index) {
        fs.Write(page_id, new_index, Offset(index_id) + ID);
    }

    /**
     * 第一个字段是Parent
     * @return
     */
public:
    Id Parent() {
        Ptr data = fs.Read(page_id).GetConstData();
        return *(Id * )(data);
    }
private:
    void Parent(Id parent) {
        fs.Write(page_id, {(Ptr) & parent, ID}, 0);
    }

    /**
     * 第二个字段是ParentIndex
     * @return
     */
public:
    Id ParentIndexId() {
        Ptr data = fs.Read(page_id).GetConstData();
        return *(Id * )(data + ID);
    }
private:
    void ParentIndexId(Id index_id) {
        fs.Write(page_id, {(Ptr) & index_id, ID}, ID);
    }

    /**
     * 第三个字段是Next
     * @return
     */
public:
    Id Next() {
        Ptr data = fs.Read(page_id).GetConstData();
        return *(Id * )(data + ID + ID);
    }
private:
    Id Next(Id next) {
        fs.Write(page_id, {(Ptr) & next, ID}, ID + ID);
        return next;
    }

    /**
     * 第四个字段是Size
     * @return
     */
public:
    Int Size() {
        auto page = fs.Read(page_id).GetConstData();
        return *(Id * )(page + ID + ID + ID);
    }
private:
    void Size(Int size) {
        fs.Write(page_id, {(Ptr) & size, ID}, ID + ID + ID);
    }

    /**
     * 第五个字段是是否为Leaf
     * @return
     */
public:
    bool IsLeaf() {
        auto page = fs.Read(page_id).GetConstData();
        return *(Id * )(page + ID * 4) == 1;
    }
private:
    bool IsLeaf(bool leaf) {
        Id is_leaf = leaf;
        fs.Write(page_id, {(Ptr) & is_leaf, ID}, ID + ID + ID + ID);
        return leaf;
    }

private:
    void SizeInc() {
        Size(Size() + 1);
    }

    void SizeSub() {
        Size(Size() - 1);
    }
public:

    void Insert(Id i, Id to_page_id, Slice index) {   // 插入一条记录使其下标为 i ，在最后插入填size
        for (Int j = Size(); j > i; j--) {
            Child(j, Child(j - 1));
            Index(j, Index(j - 1));
        } Child(i, to_page_id); Index(i, index);
        SizeInc();
        if (i == Size() - 1) SetParentIndex();
    }

    void Insert(Id i, Id child_id) {
        if (IsLeaf()) throw ERROR_CALL;
        auto c = Get(child_id, fs, index_size);
        Insert(i, child_id, c.Index(c.Size() - 1));
    }

    void Remove(Id i) { // 删除下标为 i 的记录。
        for (Int j = i; j < Size() - 1; j++) {
            Child(j, Child(j + 1));
            Index(j, Index(j + 1));
        }
        SizeSub();
        if (i == Size() && Size() != 0) SetParentIndex();
    }




    static BPlusPage New(Id parent, Id parent_index, Id next, Id prev, bool is_leaf, FileSystem& fs, Size_t index_size) {
        Id id = fs.New();
        BPlusPage p = Get(id, fs, index_size);
        p.Size(0);
        p.Parent(parent);
        p.ParentIndexId(parent_index);
        p.IsLeaf(is_leaf);
        p.Next(next);
        if (prev != NULL_ID) Get(prev, fs, index_size).Next(p.PageId());
        return p;
    }

    static void Delete(Id id, FileSystem& fs, Size_t index_size) {
        BPlusPage p = Get(id, fs, index_size);

        if (p.Parent() != NULL_ID) {
            auto parent = Get(p.Parent(), fs, index_size);
            Int p_id = p.ParentIndexId();
            if (p_id > 0) Get(parent.Child(p_id - 1), fs, index_size).Next(p.Next());
            parent.Remove(p_id);
        } else {
            if (p.Size() == 1) {
                Get(p.Child(0), fs, index_size).Parent(NULL_ID);
                Get(p.Child(0), fs, index_size).ParentIndexId(NULL_ID);
            }
        }
        fs.Delete(id);
    }

private:


    static BPlusPage Get(Id id, FileSystem& fs, Size_t index_size) {
        return BPlusPage(fs, id, index_size);
    }


    Int Offset(Id index_id) const {
        return ID + ID + ID + ID + ID + index_id * (index_size + ID);
    }


    void SetParentIndex() {
        if (Parent() != NULL_ID) {
            auto parent = Get(Parent(), fs, index_size);
            parent.Index(ParentIndexId(), Index(Size() - 1));
            if (ParentIndexId() == parent.Size() - 1) {
                parent.SetParentIndex();
            }
        }
    }

public:
    void Debug(Int height = 0, Id parent = NULL_ID) {
        if (Parent() != parent)
            throw ERROR_NULL_POINTER;
        string tab;
        for (int i = 0; i < height; i++) tab.push_back('\t');
        cout << tab << "page : " << page_id << " height " << height << " : " << endl;
        cout << tab;
        for (Int i = 0; i < Size(); i++) {
            Id x = *(Id*)Index(i).begin;
            if (x == -1)
                throw ERROR_NULL_POINTER;
            cout << x << " ";
        } cout << endl;
        for (Int i = 0; i < Size(); i++) {
            if (!IsLeaf()) {
                Get(Child(i), fs, index_size).Debug(height + 1, page_id);
            }
        }
    }
};

#endif //WALKERDB_B_PLUS_PAGE_H
