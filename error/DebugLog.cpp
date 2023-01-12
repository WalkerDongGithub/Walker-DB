//
// Created by Walker on 2022/7/19.
//

#include "DebugLog.h"

#include <cstdio>

void Log(Int level, const string& content) {
    if (level == IFO)      printf("[] %s\n", content.c_str());
    else if (level == WRN) printf("[] %s\n", content.c_str());
    else if (level == ERR) printf("[] %s\n", content.c_str());
    else throw ERROR_LOG;
}

void CatchError(Status ERROR) {
    switch (ERROR) {
        case ERROR_OUT_OF_BOUND:
            Log(ERR, "error out of bound!");
            return;
        case ERROR_IO_EXCEPTION:
            Log(ERR, "error IO failed!");
            return;
        case ERROR_LOG:
            Log(ERR, "error ,Log failed");
            return;
        case ERROR_NULL_POINTER:
            Log(ERR, "Null Pointer Exception!");
            return;
        case ERROR_CALL:
            Log(ERR, "Function Call Error, you can't call this function here.");
            return;
        case ERROR_RECORD_SIZE:
            Log(ERR, "Record Size Not Equal.");
            return;
        case ERROR_MEMORY:
            Log(ERR, "Memory too small! Please Allocate more memory.");
        default:
            return;
    }
}