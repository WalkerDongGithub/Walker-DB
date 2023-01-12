//
// Created by Walker on 2022/7/19.
//

#ifndef WALKERDB_ERRORCODE_H
#define WALKERDB_ERRORCODE_H

#include "../definition.h"
const Status OK                 = 0;
const Status ERROR_OUT_OF_BOUND = 1;
const Status ERROR_IO_EXCEPTION = 2;
const Status ERROR_NULL_POINTER = 3;
const Status ERROR_LOG          = 4;
const Status ERROR_CALL         = 5;
const Status ERROR_RECORD_SIZE  = 6;
const Status ERROR_MEMORY       = 7;
const Status ERROR_DISK         = 8;
const Int IFO = 1;
const Int WRN = 2;
const Int ERR = 3;
void Log(Int level, const string& content);
void CatchError(Status ERROR);


#endif //WALKERDB_ERRORCODE_H
