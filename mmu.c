#include "mmu.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

#define OP 1
bool canOnlyReplaceBlock(Line);

int LFU(int, Cache*);
int LRU(int, Cache*);

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
int mappingMethods(int address, Cache *cache){
    int op = OP;
    if(op == 1)
        return LFU(address, cache);
    else if(op == 2)
        return LRU(address, cache);
    
    return address % cache->size;
}

int LFU(int address, Cache *cache)
{
    int posInCache = 0;
    for (int i = 0; i < cache->size; i++){
        //Se o bloco com mesma tag esta no cache
        if (cache->lines[i].tag == address)
            return i;

        if (cache->lines[posInCache].quantUsed < cache->lines[i].quantUsed)
            posInCache = i;
    }
    return posInCache;
}

int LRU(int address, Cache *cache){
    int posInCache = 0;
    for(int i = 0; i < cache->size; i++){
        //Aumenta o tempo em cache para todas linhas
        cache->lines[i].tempInCache += 1;
        //Se o bloco com mesma tag esta no cache
        if(cache->lines[i].tag == address){
            //Aumenta o tempo das linhas depois da tag == address
            for(int j = i + 1; j < cache->size; j++){
                cache->lines[j].tempInCache += 1;
            }
            return i;
        }

        if(cache->lines[i].tempInCache > cache->lines[posInCache].tempInCache)
            posInCache = i;
    }
    cache->lines[posInCache].tempInCache += 1;
    return posInCache;
}

Line* MMUSearchOnMemorys(Address add, Machine* machine, WhereWasHit* whereWasHit) {
    // Strategy => write back
    // Direct memory map
    int l1pos = mappingMethods(add.block, &machine->l1);
    int l2pos = mappingMethods(add.block, &machine->l2);
    int l3pos = mappingMethods(add.block, &machine->l3);

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
        lineUsed->tempInCache = 0;
    } else if (cache3[l3pos].tag == add.block) { 
        /* Block is in memory cache L3 */
        cache3[l3pos].tag = add.block;
        cache3[l3pos].updated = false;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3;
        *whereWasHit = L3Hit;
        lineUsed = &(cache3[l3pos]);
        lineUsed->quantUsed += 1;
        lineUsed->tempInCache = 0;
    } else { 
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */
        l2pos = mappingMethods(cache1[l1pos].tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
        l3pos = mappingMethods(cache2[l2pos].tag, &machine->l3);
        if (!canOnlyReplaceBlock(cache1[l1pos])) { 
            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos])){ 
                    if (!canOnlyReplaceBlock(cache3[l3pos])){
                        /* The block on cache L2 cannot only be replaced, the memories must be updated */
                        RAM[cache3[l3pos].tag] = cache3[l3pos].block;
                    }
                    //Zera tempo do bloco
                    cache3[l3pos].tempInCache = 0;                     
                    //Coloca no L3 o bloco que estava no L2
                    cache3[l3pos] = cache2[l2pos];
            }
            // Zera tempo do bloco -> Recentemente usado
            cache2[l2pos].tempInCache = 0;
            // Coloca no L2 o bloco que estava no L1
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
        // Zera tempo do bloco -> Recentemente usado
        lineUsed->tempInCache = 0;
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
