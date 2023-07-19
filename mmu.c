#include "mmu.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

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
int memoryCacheMapping(int address, Cache *cache)
{
    //LFU
    int posInCache = 0;
    for (int i = 0; i < cache->size; i++)
        {
        if ( cache->lines[posInCache].quantUsed > cache->lines[i].quantUsed)
            posInCache = i;

        if (cache->lines[i].tag == address)
            return i;
    }
    return posInCache;
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
    Line* lineUsed = NULL;
    MemoryBlock* RAM = machine->ram.blocks;
    int cost = 0;


    if (cache1[l1pos].tag == add.block) { 
        /* Block is in memory cache L1 */
        cost = COST_ACCESS_L1;
        *whereWasHit = L1Hit;
    } else if (cache2[l2pos].tag == add.block) { 
        /* Block is in memory cache L2 */
        cache2[l2pos].tag = add.block;
        cache2[l2pos].updated = false;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2;
        *whereWasHit = L2Hit;
        lineUsed = &(cache2[l2pos]);
        lineUsed->quantUsed += 1;
    } else if (cache3[l3pos].tag == add.block) { 
        /* Block is in memory cache L3 */
        cache3[l3pos].tag = add.block;
        cache3[l3pos].updated = false;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3;
        *whereWasHit = L3Hit;
        lineUsed = &(cache3[l3pos]);
        lineUsed->quantUsed += 1;
    } else { 
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */
        l2pos = memoryCacheMapping(cache1[l1pos].tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
        l3pos = memoryCacheMapping(cache2[l2pos].tag, &machine->l3);
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
    /*  Se o bloco nao estiver em L1/L2/L3 -> primeiros IF's,
        lineUsed esta com o valor inicial (NULL), atribui entao
        cache1[l1pos] para ele
    */

    if(lineUsed == NULL){
        lineUsed = &(cache1[l1pos]);
        lineUsed->quantUsed += 1;
    }
    updateMachineInfos(machine, whereWasHit, cost);
    return lineUsed;
}

bool canOnlyReplaceBlock(Line line) {
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (line.tag == INVALID_ADD || (line.tag != INVALID_ADD && !line.updated))
        return true;
    return false;
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
