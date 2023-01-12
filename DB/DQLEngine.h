//
// Created by 董文阔 on 2022/9/25.
//

#ifndef WALKERDB_DQLENGINE_H
#define WALKERDB_DQLENGINE_H

#include "File.h"
#include <stack>
#include "../Query/Projection.h"
#include "../Query/Select.h"
#include "../Query/Sort.h"

class DQLEngine {
public:

    struct DQLEngineIterator {

        struct ColumnInfo {

            ColumnInfo() {}

            string name;
            string type;
            int begin;
            int size;
            ColumnInfo(const string& name, const string& type, int begin, int size)
                    : name(name), type(type), begin(begin), size(size) {
            }

            ColumnInfo(const ColumnInfo&) = default;


        };
        Iterable* iterable;
        bool fromFile;
        map<string, ColumnInfo> infos;
        DQLEngineIterator(Iterable* iterable, const map<string, ColumnInfo>& infos, bool fromFile = false):
                iterable(iterable), infos(infos), fromFile(fromFile) {}

    };

    FileSystem& fs;


public:

    DQLEngine(FileSystem& fs) : fs(fs) {}

    /**
     * 从文件中获取一个带列名信息的iterator。
     * 注意这里的iterator里的真实用来迭代的iterator用完要delete
     * @param file
     * @return
     */
public:
    stack<DQLEngineIterator> GetFromFile(File* file) {
        Iterable* iterable = file->GetIterable();
        map<string, DQLEngineIterator::ColumnInfo> columnInfos;
        int num = stoi(file->GetConfig("column_num"));
        for (int i = 0; i < num; i++) {
            string name = file->GetConfig("column_" + to_string(i) + "_name");
            DQLEngineIterator::ColumnInfo info(
                    name,
                    file->GetConfig("column_" + to_string(i) + "_type"),
                    stoi(file->GetConfig("column_" + to_string(i) + "_begin")),
                    stoi(file->GetConfig("column_" + to_string(i) + "_size"))
            );
            columnInfos[name] = info;
        }
        stack<DQLEngineIterator> s;
        s.push(DQLEngineIterator(iterable, columnInfos, true));
        return s;
    }


    /**
     * select name, id
     * from student
     * where age >= 18 and gender = 0
     * order by age; 一个例子
     * 这个函数就是将这个执行栈启动执行
     * 栈中保存了有关于query的执行的先后顺序
     * 取栈顶元素的iterable的迭代器，就可以获得查询结果
     * @param executeStack 执行栈
     * @return
     */
    vector<map<string, string>> Return(stack<DQLEngineIterator>& executeStack) {
        DQLEngineIterator iterator = executeStack.top();
        vector<map<string, string>> items;
        Iterator* it = iterator.iterable->GenerateIterator();
        while (!it->Null()) {
            Slice s = it->Get();
            map<string, string> item;
            for (const auto& x : iterator.infos) {
                if (x.second.type == "int") {
                    Byte buf[16]{};
                    memcpy(buf, s.begin + x.second.begin, x.second.size);
                    int i = *(Int *) buf;
                    item[x.first] = to_string(i);
                } else if (x.second.type == "varchar") {
                    item[x.first] = string((char*)s.begin, x.second.begin, x.second.size);
                }
            } items.push_back(item);
            it->Next();
        }
        while (!executeStack.empty()) {
            auto x = executeStack.top();
            if (x.fromFile == false) {
                delete x.iterable;
            } executeStack.pop();
        }
        delete it;
        return items;
    }

    /**
     * 投影运算调用，获取一组行名，然后根据行名来获取投影
     * @param columns
     * @param executeStack
     */
    void Projection(const vector<string>& columns, stack<DQLEngineIterator>& executeStack) {
        map<string, DQLEngineIterator::ColumnInfo> infos;
        int all_begin = 0;
        DQLEngineIterator in = executeStack.top();
        IndexPart part;
        for (const auto& x : columns) {
            if (in.infos.find(x) != in.infos.end()) {
                auto y = *in.infos.find(x);
                part.range[part.index_num++] = {
                        y.second.begin, y.second.size
                };
                infos[x] = DQLEngineIterator::ColumnInfo(
                        y.second.name,
                        y.second.type,
                        all_begin,
                        y.second.size
                );
                all_begin += y.second.size;
            }
        }
        Iterable* iterable = new ProjectionQuery(*executeStack.top().iterable, part);
        executeStack.push(DQLEngineIterator(iterable, infos));
    }

    /**
     * Condition，where的条件
     */
    struct Condition {
        string name;
        string op;
        string value;

        Filter* GetFilter(DQLEngineIterator iterator) const {
            IndexPart part;
            if (iterator.infos.find(name) != iterator.infos.end()) {
                auto x = *iterator.infos.find(name);
                part.range[part.index_num++] = {x.second.begin, x.second.size};
            };
            if (iterator.infos[name].type == "int") {
                CompareIntegerFilter *filter = new CompareIntegerFilter();
                filter->part = part;
                filter->value = stoi(value);
                if (op == "<")
                    filter->type = CompareIntegerFilter::Type::LESS;
                else if (op == ">") {
                    filter->type = CompareIntegerFilter::Type::GREATER;
                } else if (op == "=") {
                    filter->type = CompareIntegerFilter::Type::EQUAL;
                } else if (op == ">=") {
                    filter->type = CompareIntegerFilter::Type::GREATER_EQUAL;
                }  return filter;
            } else if (iterator.infos[name].type == "varchar") {
                return NULL;
            }
            return NULL;
        }

    };

    // 目前只写一个age < 18 and gender = 0吧，简单一点


    /**
     * 整数比较的Filter
     */
    struct CompareIntegerFilter : public Filter {
        IndexPart part;
        long long value;
        enum Type {
            GREATER, LESS, EQUAL, LESS_EQUAL, GREATER_EQUAL, NO_EQUAL
        };
        Type type;
        bool f(const Slice & a) override {
            long long r = GetIndex(a, part).GetInteger();
            switch (type) {
                case GREATER:
                    return r > value;
                case LESS:
                    return r < value;
                case EQUAL:
                    return r == value;
                case NO_EQUAL:
                    return r != value;
                case LESS_EQUAL:
                    return r <= value;
                case GREATER_EQUAL:
                    return r >= value;
            } return false;
        }
    };

    /**
     * 与运算Filter
     */
    struct AndFilter : public Filter {
        Filter* filter_a;
        Filter* filter_b;
        AndFilter(Filter* a, Filter* b) {
            filter_a = a;
            filter_b = b;
        }
        bool f(const Slice & a) override {
            return filter_a->f(a) && filter_b->f(a);
        }
    };

    /**
     * 选择操作调用
     * @param filter        选择操作使用的Filter
     * @param executeStack  执行栈
     */
    void Select(Filter* filter, stack<DQLEngineIterator>& executeStack) {
        DQLEngineIterator it = executeStack.top();
        Iterable* iterable = new SelectQuery(*it.iterable, filter);
        executeStack.push(DQLEngineIterator(iterable, it.infos));
    }


    /**
     * 排序操作调用
     * @param column         排序使用的列名
     * @param executeStack   执行栈
     */
    void Sort(const string& column, stack<DQLEngineIterator>& executeStack) {
        DQLEngineIterator it = executeStack.top();
        IndexPart part = GetIndexPart(column, it);
        Compare c = (it.infos.find(column)->second.type == "int") ? integer_compare : string_compare;
        Iterable* iterable = new SortQuery(*it.iterable, c, part);
        DQLEngineIterator out(iterable, it.infos);
        executeStack.push(out);
    }

private:

    IndexPart GetIndexPart(const string& column, DQLEngineIterator it) {
        IndexPart part;
        if (it.infos.find(column) != it.infos.end()) {
            auto x = *it.infos.find(column);
            part.range[part.index_num++] = {x.second.begin, x.second.size};
        } return part;
    }




};


#endif //WALKERDB_DQLENGINE_H
