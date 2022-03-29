//
// Created by zhaobo on 2022/3/28.
//
#include "simple_mem_pool.h"

int main(int argc, char** argv) {
    SimpleMemPool* simpleMemPool = new SimpleMemPool(100);
    char* block1 = simpleMemPool->request(10);
    char* block2 = simpleMemPool->request(20);
    char* block3 = simpleMemPool->request(30);
    simpleMemPool->release(block2, 20);
    simpleMemPool->release(block3, 30);
    simpleMemPool->release(block1, 10);
    char* block4 = simpleMemPool->request(20);
    char* block5 = simpleMemPool->request(15);
    char* block6 = simpleMemPool->request(30);
    simpleMemPool->release(block4, 20);
    simpleMemPool->infos();
    delete simpleMemPool;
    return 0;
}
