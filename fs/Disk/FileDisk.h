//
// Created by Walker on 2022/7/18.
//

#ifndef WALKERDB_FILEDISK_H
#define WALKERDB_FILEDISK_H

#include "Disk.h"
#include <cstdio>

/**
 * 这个实现本身就是跨平台的，但是效率并不算高，需要优化。
 * 但是优化方案也是显而易见的把跨平台的接口换成平台专用接口。
 * 所以就放在这了，有时间再改。
 */
class FileDisk : public Disk {
private:
    FILE* fp;
    Page white_page;
    Int max_id;
public:

    FileDisk(char file_name[] = NULL) {
        if (file_name == NULL) return;
        fp = fopen(file_name, "ab+");
        if (fp == NULL)
            throw 2;
        fseek(fp, 0, SEEK_END);
        max_id = ftell(fp) / PAGE_SIZE;
        fclose(fp);
        fp = fopen(file_name, "rb+");
        if (fp == NULL)
            throw 2;
    }

    Status Read(Id i, Page &page) override {

        if (i >= max_id)
            throw ERROR_OUT_OF_BOUND;
        if (fseek(fp, i * PAGE_SIZE, 0) != 0)
            throw 1;
        if (fread(page.GetData(), PAGE_SIZE, 1, fp) != 1)
            throw 1;
        fflush(fp);
        return OK;
    }
    Status Write(Id i, Page &page) override {

        if (i >= max_id)
            throw ERROR_OUT_OF_BOUND;
        if (fseek(fp, i * PAGE_SIZE, 0) != 0)
            throw 1;
        if (fwrite(page.GetData(), PAGE_SIZE, 1, fp) != 1)
            throw 1;
        fflush(fp);
        return OK;
    }
    Status New(Id &i) override {
        i = max_id ++;
        Write(i, white_page);
        return OK;
    }

    Size_t Size() override {
        return max_id;
    }
};

#endif //WALKERDB_FILEDISK_H
