//
// Created by 董文阔 on 2022/7/16.
//

#ifndef WALKERDB_BUFFER_H
#define WALKERDB_BUFFER_H

#include <vector>
#include "../../definition.h"
/**
 * Buffer，即缓冲区，2022/09之前都只有全局LRU和无Buffer两种设计。
 * 其目的就是和磁盘形成一个缓冲，从而减少对磁盘的访问。
 * 当然他的最重要的任务实际就是保证短期内某磁盘页可以保持在内存中，
 * 例如在一次例如B+树调整过程中大量使用到磁盘访问，此时应尽可能保证这些页都在内存中没有丢失。
 * todo： 这里指保证了一个经常使用的磁盘块会一直保持在内存中。
 * todo： 但是没有保证事务间隔离，因为事务还没写。
 * todo： 而且总觉得速度还有缺失，毕竟是一个非常频繁需要使用的接口。
 */
class Buffer {
public:
    /**
     * 调用Buffer置换算法，计算结果返回。
     * @param i                 需要从Buffer中取出的物理页号
     * @param replace_key       若发生置换，则置换出去的物理页号
     * @param replace_value     若发生置换，则置换出去的内存页号
     * @param miss              是否发生缺页，发生缺页有可能发生置换，也可能不发生
     * @return                  返回物理页号对应的内存页号
     */
    virtual Id Get(Id i, Id& replace_key, Id& replace_value, bool& miss) = 0;


    /**
     * 获取内存中的所有内存页号，用于全部写回操作
     * @return 返回所有的内存页号。
     */
    virtual vector<Id> GetAllPagesInBuffer() const = 0;
};


#endif //WALKERDB_BUFFER_H
