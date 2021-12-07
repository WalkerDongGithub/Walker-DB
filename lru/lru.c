//
// Created by 董文阔 on 2021/12/7.
//
#include "lru.h"







static void stk_init(stk* s) {
    s->top = 0;
}
static void stk_push(stk* s, int v) {
    s->s[s->top++] = v;
}
static int stk_pop(stk* s) {
    s->top--;
    return s->s[s->top];
}
static int stk_empty(stk* s) {
    return s->top == 0;
}









static int h_code(int page_id) {
    return page_id % hash_bkt;
}


static int lru_select(lru* pLru, int page_id) {

    // 哈希表部分
    int c = h_code(page_id);
    int p;
    for (p = pLru->bkt[c].head; p != l_end; p = pLru->h_nodes[p].next) {
        if (pLru->h_nodes[p].page_id == page_id) { // 找到page id对应的hash表的位置。
            break;
        }
    }
    if (p != l_end) {
        p = pLru->h_nodes[p].ptr;
    } else {
        return l_end;
    }


    // 链表部分
    // 此时p为page_id在链表中所处的位置，将其置于首部
    if (p != pLru->head) {
        d_node *t = &pLru->d_nodes[p];

        int prev = t->prev; // 解绑
        int next = t->next;
        if (prev != l_end) pLru->d_nodes[prev].next = next;
        if (next != l_end) pLru->d_nodes[next].prev = prev;
        if (pLru->head == p) pLru->head = next;
        if (pLru->tail == p) pLru->tail = prev;

        int head = pLru->head;
        t->next = head; // 置顶
        pLru->d_nodes[head].prev = p;
        pLru->head = p;
    }
    return p;


}

static void lru_pop(lru* pLru) {

    int page_id = pLru->d_nodes[pLru->tail].page_id;


    //链表部分
    stk_push(&pLru->d_es, pLru->tail);
    pLru->tail = pLru->d_nodes[pLru->tail].prev;
    pLru->d_nodes[pLru->tail].next = l_end;
    if (pLru->tail == l_end) pLru->head = l_end;

    // 哈希表部分
    int c = h_code(page_id);

    int p;
    for (p = pLru->bkt[c].head; p != l_end; p = pLru->h_nodes[p].next) {
        if (pLru->h_nodes[p].page_id == page_id) { // 找到page id对应的hash表的位置。
            h_node* t = &pLru->h_nodes[p];
            int prev = t->prev; // 解绑
            int next = t->next;
            if (prev != l_end) pLru->h_nodes[prev].next = next;
            if (next != l_end) pLru->h_nodes[next].prev = prev;
            if (p == pLru->bkt[c].tail) pLru->bkt[c].tail = prev;
            if (p == pLru->bkt[c].head) pLru->bkt[c].head = next;

            stk_push(&pLru->h_es, p);
            pLru->size--;
            return;
        }
    }
    printf("false!\n");
}


static void lru_push(lru* pLru, int page_id) {

    // 链表部分
    int dn = stk_pop(&pLru->d_es);
    if (stk_empty(&pLru->d_es) && pLru->size < lru_max-1) stk_push(&pLru->d_es, pLru->size + 1);
    d_node node = {page_id, pLru->head, l_end};
    pLru->d_nodes[dn] = node;
    if (pLru->head != l_end) pLru->d_nodes[pLru->head].prev = dn;
    pLru->head = dn;

    if (pLru->tail == l_end) pLru->tail = dn;


    // 哈希表部分
    int hn = stk_pop(&pLru->h_es);
    if (stk_empty(&pLru->h_es) && pLru->size < lru_max-1) stk_push(&pLru->h_es, pLru->size + 1);
    int c = h_code(page_id);
    h_node node2 = {page_id, dn, pLru->bkt[c].head, l_end};
    pLru->h_nodes[hn] = node2;
    if (pLru->bkt[c].head != l_end) pLru->h_nodes[pLru->bkt[c].head].prev = dn;
    pLru->bkt[c].head = dn;
    if (pLru->bkt[c].tail == l_end) pLru->bkt[c].tail = dn;

    pLru->size++;


}

static int lru_add(lru* l, int page_id) {
    if (l->size == lru_max) {
        lru_pop(l);
    }
    lru_push(l, page_id);

    return l->head;
}

int lru_get(lru *l, int page_id) {
    int p = lru_select(l, page_id); // 这里的 p 是 page_id 在 双向队列 中对应的位置
    if (p == l_end) {
        p = lru_add(l, page_id);
    } return p;
}

void lru_init(lru *pLru) {
    pLru->size = 0;
    h_list l = {l_end, l_end};
    for (int i = 0; i < hash_bkt; i++) {
        pLru->bkt[i] = l;
    }

    stk_init(&pLru->d_es);
    stk_push(&pLru->d_es, 0);
    stk_init(&pLru->h_es);
    stk_push(&pLru->h_es, 0);

    pLru->head = pLru->tail = l_end;

}
