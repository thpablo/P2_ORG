#include "mmu.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

/* Fila para FIFO 
typedef struct{
    int posCacheArray;
    Line lineInPosCache;
} PosCache;

typedef struct {
    PosCache *arrayPos;
    int start, end;
    int tamVet;
} ArrayCache;

//tamVet = machine->l1/2
ArrayCache *startQueue(int tamVet){
    ArrayCache *queue = (ArrayCache *) malloc(sizeof(ArrayCache));
    queue->arrayPos = (PosCache *) malloc(sizeof(PosCache) * tamVet);
    queue->start = 0;
    queue->end = 0;
    queue->tamVet = tamVet;

    return queue;
}


void enqueue(ArrayCache *queue, PosCache item) {
    if ((queue->end + 1) % queue->tamVet == queue->start) {
        printf("Fila cheia. Não é possível adicionar mais elementos.\n");
        return;
    }

    queue->arrayPos[queue->end] = item;
    queue->end = (queue->end + 1) % queue->tamVet;
}

PosCache dequeue(ArrayCache *queue) {
    if (queue->start == queue->end) {
        printf("Fila vazia. Não é possível remover elementos.\n");
        PosCache emptyPosCache;
        emptyPosCache.posCacheArray = -1;
        return emptyPosCache;
    }

    PosCache item = queue->arrayPos[queue->start];
    queue->start = (queue->start + 1) % queue->tamVet;
    return item;
}
*/
bool canOnlyReplaceBlock(Line);

int memoryCacheMapping(int, Cache*);

int lineWhichWillLeave(int, Cache*);

void updateMachineInfos(Machine*, WhereWasHit*, int);

void moveLine();

char* convertToString(WhereWasHit whereWasHit) {
    switch (whereWasHit) {
        case L1Hit:
            return "CL1";
        case L2Hit:
            return "CL2";
        case L3Hit:
            return "CL3";
        case RAMHit:
            return "RAM";
    }
}

Line* MMUSearchOnMemorys(Address add, Machine* machine, WhereWasHit* whereWasHit) {
    // Strategy => write back
    // Direct memory map
    int l1pos = memoryCacheMapping(add.block, &machine->l1);
    int l2pos = memoryCacheMapping(add.block, &machine->l2);
    int l3pos = memoryCacheMapping(add.block, &machine->l3);

    Line* cache1 = machine->l1.lines;
    Line* cache2 = machine->l2.lines;
    Line* cache3 = machine->l3.lines;
    MemoryBlock* RAM = machine->ram.blocks;
    int cost = 0;


    if (cache1[l1pos].tag == add.block) { 
        /* Block is in memory cache L1 */
        cost = COST_ACCESS_L1;
        *whereWasHit = L1Hit;
    } else if (cache2[l2pos].tag == add.block) { 
        /* Block is in memory cache L2 */
        cache2[l2pos].tag = add.block;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2;
        *whereWasHit = L2Hit;
        // Just works for Direct Mapping
        {
            Line tmp = cache1[l1pos];
            cache1[l1pos] = cache2[l2pos];
            int l2pos = lineWhichWillLeave(tmp.tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
            if (!canOnlyReplaceBlock(cache2[l2pos])) { 
                RAM[cache2[l2pos].tag] = cache2[l2pos].block;
            }
            cache2[l2pos] = tmp;
        }         
    } else if (cache3[l3pos].tag == add.block) { 
        /* Block is in memory cache L3 */
        cache3[l3pos].tag = add.block;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3;
        *whereWasHit = L3Hit;
        // Just works for Direct Mapping
        {
            Line tmp2 = cache2[l2pos];
            cache2[l2pos] = cache3[l3pos];
            int l3pos = lineWhichWillLeave(tmp2.tag, &machine->l3); /* Need to check the position of the block that will leave the L1 */
            if (!canOnlyReplaceBlock(cache3[l3pos])) { 
                RAM[cache3[l3pos].tag] = cache3[l3pos].block;
            }
            cache3[l3pos] = tmp2;
        }
    } else { 
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */
        l2pos = lineWhichWillLeave(cache1[l1pos].tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
        l3pos = lineWhichWillLeave(cache2[l2pos].tag, &machine->l3);
        if (!canOnlyReplaceBlock(cache1[l1pos])) { 
            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos])){ 
                    if (!canOnlyReplaceBlock(cache3[l3pos])){
                        /* The block on cache L2 cannot only be replaced, the memories must be updated */
                        RAM[cache3[l3pos].tag] = cache3[l3pos].block;
                    }
                    cache3[l3pos] = cache2[l2pos];
            }
            cache2[l2pos] = cache1[l1pos];
        }
        cache1[l1pos].block = RAM[add.block];
        cache1[l1pos].tag = add.block;
        cache1[l1pos].updated = false;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3 + COST_ACCESS_RAM;
        *whereWasHit = RAMHit;
    }
    updateMachineInfos(machine, whereWasHit, cost);
    return &(cache1[l1pos]);
}

bool canOnlyReplaceBlock(Line line) {
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (line.tag == INVALID_ADD || (line.tag != INVALID_ADD && !line.updated))
        return true;
    return false;
}

int memoryCacheMapping(int address, Cache* cache) {
    return address % cache->size;
}

int lineWhichWillLeave(int address, Cache* cache) {
    return address % cache->size;
}

void updateMachineInfos(Machine* machine, WhereWasHit* whereWasHit, int cost) {
    switch (*whereWasHit) {
        case L1Hit:
            machine->hitL1 += 1;
            break;

        case L2Hit:
            machine->hitL2 += 1;
            machine->missL1 += 1;
            break;
        
        case L3Hit:
            machine->hitL3 += 1;
            machine->missL1 += 1;
            machine->missL2 += 1;
            break;
        case RAMHit:
            machine->hitRAM += 1;
            machine->missL1 += 1;
            machine->missL2 += 1;
            machine->missL3 += 1;
            break;
    }
    machine->totalCost += cost;
}