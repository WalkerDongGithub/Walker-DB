//
// Created by 董文阔 on 2021/11/30.
//

#include "walker_context.h"


void debug(walker_string info, walker_status level) {
    if (level == walker_error) {
        printf("%s error : %d : %s \n", info, errno, strerror(errno));
    }
}