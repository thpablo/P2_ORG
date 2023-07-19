#include "memory.h"

void startRAM(RAM* ram, int size) {
    ram->blocks = (MemoryBlock*) malloc(sizeof(MemoryBlock) * size);
    ram->size = size;

    for (int i=0;i<size;i++) {
        for (int j=0;j<WORDS_SIZE;j++)
            ram->blocks[i].words[j] = rand() % 100;
            //ram->blocks[i].words[j] = -1;            
    }
}

void stopRAM(RAM *ram) {
    free(ram->blocks);
}


void startCache(Cache* cache, int size) {
    cache->lines = (Line*) malloc(sizeof(Line) * size);
    cache->size = size;

    for (int i=0;i<size;i++){
        cache->lines[i].tag = INVALID_ADD;
        cache->lines[i].quantUsed = 0;
    }

}

void stopCache(Cache *cache) {
    free(cache->lines);
}
