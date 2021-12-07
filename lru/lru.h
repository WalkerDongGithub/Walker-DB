//
// Created by Walker on 2021/12/7.
//

#ifndef WSQL_LRU_H
#define WSQL_LRU_H


/**
 * LRU算法：
 * 页面置换算法的其中一种比较常用的办法：
 *
 *          大抵是维护两个数据结构：哈希表和双向链表，两种数据结构相互配合实现LRU
 *
 *          当上级请求调页时，查询链表中是否存在该页的缓存，如果有，则直接在页表中返回该页的指针，并将该页置顶。
 *          如果没有，则将请求页插入到LRU链表中，如果超过了LRU链表的维护长度，则将链表的尾部删去，即删除最不常用的页。
 *
 *          而查询操作如果只使用链表则耗时太久，故采用哈希技术来实现页表查询、增删操作。
 *          考虑增删操作过于频繁，基于顺序表的哈希表效率实在太低，故选择了基于双向链表的哈希表的方案。
 *
 *          具体实现细节见lru.c
 *
 *
 *
 */






#include "../walker_context.h"
#define l_end  -1
#define hash_bkt  397       // 哈希表桶的大小
#define lru_max   40        // LRU链表最大长度。

typedef struct d_node d_node;
typedef struct h_node h_node;
typedef struct h_list h_list;
typedef struct stk stk;
typedef struct lru lru;

/**
 * stk 基本栈结构，此处的栈结构用于维护两个数据结构的空区页表
 *
 * 具体逻辑为：
 *              （x可以是哈希表，也可以是链表）
 *              x 的所有使用的空间被组织成一个数组
 *              当 x 需要一块新的空间时，从stk栈的栈顶取一个下标给 x，然后如果栈空，则将重新寻找一块空的空间的下标。
 *              当 x 不再需要某空间时，x 应该将该空间下标压入栈中，供需要新空间时分配。
 */
struct stk {
    int top;
    int s[lru_max];
};



/**
 *  以 h 开头为哈希表结构之意，
 *  h_node维护一个从磁盘页面号到内存页面号的映射对，
 *  并作为双向链表的节点维护前驱后继指针。
 */
struct h_node {
    int page_id;
    int ptr;
    int next;
    int prev;
};
struct h_list {
    int head;
    int tail;
};

/**
 * 以 d 开头为双向链表结构之意，双向链表结构直接嵌入到了LRU中，未封装。
 * 链表中存储的号为磁盘页面的页面号，其在数组中的下标作为他的内存页面号。
 *
 *
 */
struct d_node {
    int page_id;
    int next;
    int prev;
};




/**
 * LRU 哈希链表数据结构，用于实现页面置换算法的数据结构。
 * 最开始的设计是哈希表和链表分离的，但是后来实践证明两者的耦合度非常强，基本无法将二者分离，
 * 而且实践结果表明链表是严格为哈希表服务的，而非哈希表为链表服务。
 * 链表实际是为本无序的哈希表定义了一个顺序，即LRU页面置换规定的顺序。
 *
 */
struct lru {

    int    size;

    // 哈希表结构
    h_list bkt[hash_bkt];  // hash 桶，是一个双向链表，桶中记录该链表的头指针和尾指针。

    h_node h_nodes[lru_max];
    stk    h_es;


    // LRU链表结构
    int    head;
    int    tail;

    d_node d_nodes[lru_max];
    stk    d_es;

};

/**
 *       外部只能访问两个接口，
 *       对数据结构进行初始化的init接口
 *       和页面置换访问时LRU数据结构为调用者返回的内存页面号。
 */
void lru_init(lru*);
int  lru_get (lru*, int page_id);

#endif //WSQL_LRU_H
