//
// Created by zhaobo on 2022/3/28.
//
#pragma once

#include <stdlib.h>
#include <iostream>
#include <cmath>

using namespace std;

#define POOL_SIZE   1000

// 内存空闲块
struct mem_block {
    int pos;
    int size;
    mem_block* next;
};

class SimpleMemPool {
private:
    int start;
    int end;
    mem_block* free_list_head;      // 单链表维护内存空闲块，且空闲块地址依次递增
    char* alloc;

public:
    SimpleMemPool(int size = POOL_SIZE) : start(0), end(size), free_list_head(nullptr) {
        alloc = (char*)malloc(size);
    }

    ~SimpleMemPool() {
        while (free_list_head != nullptr) {
            mem_block* tmp = free_list_head->next;
            delete (free_list_head);
            free_list_head = tmp;
        }
        free(alloc);
    }

    SimpleMemPool(const SimpleMemPool& src) {
        start = src.start;
        end = src.end;
        free_list_head = src.free_list_head;
        alloc = src.alloc;
    }

    SimpleMemPool& operator=(const SimpleMemPool& src) {
        this->start = src.start;
        this->end = src.end;
        this->free_list_head = src.free_list_head;
        this->alloc = src.alloc;
        return *this;
    }

    char* request(int size) {
        // 1.先从空闲列表里查找：找一块不小于size大小的最小空闲块
        int selectSize = INT_MAX;
        int selectPos = -1;
        mem_block* cur = free_list_head;
        while (cur != nullptr) {
            if (cur->size >= size && cur->size < selectSize) {
                selectSize = cur->size;
                selectPos = cur->pos;
            }
            cur = cur->next;
        }
        // 空闲块获取成功
        if (selectPos != -1) {
            // 大小正好相等
            if (selectSize == size) {
                // 如果是头节点
                if (free_list_head->pos == selectPos) {
                    mem_block *cur = free_list_head;
                    free_list_head = free_list_head->next;
                    delete (cur);
                } else {
                    mem_block *pre = free_list_head;
                    mem_block *cur = pre->next;
                    while (cur != nullptr) {
                        if (cur->pos == selectPos) {
                            pre->next = cur->next;
                            delete (cur);
                        }
                    }
                }
            } else {
                // 如果是头节点
                if (free_list_head->pos == selectPos) {
                    free_list_head->pos += size;
                    free_list_head->size -= size;
                } else {
                    mem_block* cur = free_list_head->next;
                    while (cur != nullptr) {
                        if (cur->pos == selectPos) {
                            cur->pos += size;
                            cur->size -= size;
                        }
                    }
                }
            }
            return (char*)(alloc + selectPos);
        } else {    // 2.空闲列表没有合适的，从内存池中获取
            if (start + size < end) {
                char* addr = (char*)(alloc + start);
                start += size;
                return addr;
            } else {
                cout << "request error." << endl;
            }
        }
        return nullptr;
    }

    // 释放空闲块：合并相连块
    void release(char* block, int size) {
        int pos = block - alloc;
        memset(block, 0, size);
        if (free_list_head == nullptr) {    // 空闲链表为空
            free_list_head = new mem_block;
            free_list_head->pos = pos;
            free_list_head->size = size;
        } else {
            if (pos < free_list_head->pos) {      // 小于头节点pos值
                if (pos + size == free_list_head->pos) {    // 待释放block尾端正好与头节点相邻
                    free_list_head->pos = pos;
                    free_list_head->size += size;
                } else {
                    mem_block* tmp = new mem_block;
                    tmp->pos = pos;
                    tmp->size = size;
                    tmp->next = free_list_head;
                    free_list_head = tmp;
                }
            } else {
                mem_block* pre = free_list_head;
                mem_block* cur = pre->next;
                // 寻找第一块pos大于待释放块pos的空闲块
                while (cur != nullptr) {
                    if (pos > cur->pos) {
                        break;
                    }
                    pre = cur;
                    cur = cur->next;
                }
                // 待释放块插入链表中间
                if (cur != nullptr) {
                    // 插入当前块后，前后两块正好相连
                    if (pre->pos+pre->size == pos && pre->pos+pre->size+size == cur->pos) {
                        pre->size += size + cur->size;
                        delete(cur);
                    } else if (pos+size == cur->pos){    // 后一块相连
                        cur->pos = pos;
                        cur->size += size;
                    } else if (pre->pos+pre->size == pos) {   // 前一块相连
                        pre->size += size;
                    } else {       // 均不相连
                        mem_block* tmp = new mem_block;
                        tmp->pos = pos;
                        tmp->size = size;
                        tmp->next = cur;
                        pre->next = tmp;
                    }
                } else {    // 插入链表尾端
                    if (pre->pos+pre->size == pos) {    // 相连
                        pre->size += size;
                    } else {        // 不相连
                        mem_block* tmp = new mem_block;
                        tmp->pos = pos;
                        tmp->size = size;
                        tmp->next = nullptr;
                        pre->next = tmp;
                    }
                }
            }
        }
    }

    void infos() {
        int nums = 1;
        cout << "mem-pool size : " << end << " bytes" << endl;
        cout << "mem-pool cur : " << start << endl;
        cout << "free list : " << endl;
        while (free_list_head != nullptr) {
            cout << "block " << nums << ", pos : " << free_list_head->pos << ", size : "
                 << free_list_head->size << endl;
            free_list_head = free_list_head->next;
            nums++;
        }
    }
};
