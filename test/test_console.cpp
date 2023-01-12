//
// Created by 董文阔 on 2022/9/23.
//
#include "Console.h"
#include "file_system/MemoryFileSystem.h"
static MemoryFileSystem fs;
int main() {
    DDLEngine ddl(fs);
    DQLEngine dql(fs);
    Console console(ddl, dql);
    console.Run();
    return 0;
}