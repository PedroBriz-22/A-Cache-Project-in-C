// Author: Pedro Brizuela Kury
// Project 3 - Cache Simulator
// Description: Simulates cache reads/writes using LRU replacement policy
#define _CRT_SECURE_NO_WARNINGS //for Visual Studio compiler
#pragma warning(disable:6031) //ignore scanf warnings


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ========== STRUCT DEFINITIONS ==========
typedef struct {
    int valid;
    unsigned long tag;
    int lruCounter;
} CacheBlock;

typedef struct {
    int blockSize;
    int numSets;
    int associativity;
    int numBlocks;
    CacheBlock **blocks;
    int reads;
    int writes;
    int hits;
    int misses;
} Cache;

// ========== FUNCTION DECLARATIONS ==========
Cache* initCache(int blockSize, int cacheSize, int associativity);
void freeCache(Cache* cache);
int accessCache(Cache* cache, unsigned long address);
unsigned long hexToDecimal(char* hex);
void runCacheSimulation(char* filename, int blockSize, int cacheSize, int associativity);

// ========== FUNCTION IMPLEMENTATIONS ==========
Cache* initCache(int blockSize, int cacheSize, int associativity) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    cache->blockSize = blockSize;
    cache->associativity = associativity;
    cache->numBlocks = cacheSize / blockSize;
    cache->numSets = cache->numBlocks / associativity;
    cache->reads = 0;
    cache->writes = 0;
    cache->hits = 0;
    cache->misses = 0;

    cache->blocks = (CacheBlock**)malloc(sizeof(CacheBlock*) * cache->numSets);
    for (int i = 0; i < cache->numSets; i++) {
        cache->blocks[i] = (CacheBlock*)malloc(sizeof(CacheBlock) * associativity);
        for (int j = 0; j < associativity; j++) {
            cache->blocks[i][j].valid = 0;
            cache->blocks[i][j].tag = 0;
            cache->blocks[i][j].lruCounter = 0;
        }
    }
    return cache;
}

void freeCache(Cache* cache) {
    for (int i = 0; i < cache->numSets; i++) {
        free(cache->blocks[i]);
    }
    free(cache->blocks);
    free(cache);
}

int accessCache(Cache* cache, unsigned long address) {
    int blockOffsetBits = log2(cache->blockSize);
    int indexBits = log2(cache->numSets);
    unsigned long indexMask = (1 << indexBits) - 1;
    int index = (address >> blockOffsetBits) & indexMask;
    unsigned long tag = address >> (blockOffsetBits + indexBits);

    // HIT check
    for (int i = 0; i < cache->associativity; i++) {
        CacheBlock* block = &cache->blocks[index][i];
        if (block->valid && block->tag == tag) {
            block->lruCounter = 0;
            cache->hits++;
            for (int j = 0; j < cache->associativity; j++) {
                if (j != i && cache->blocks[index][j].valid) {
                    cache->blocks[index][j].lruCounter++;
                }
            }
            return 1;
        }
    }

    // MISS - replace block using LRU
    int lruIndex = 0;
    int maxLRU = -1;
    for (int i = 0; i < cache->associativity; i++) {
        if (!cache->blocks[index][i].valid) {
            lruIndex = i;
            break;
        }
        if (cache->blocks[index][i].lruCounter > maxLRU) {
            maxLRU = cache->blocks[index][i].lruCounter;
            lruIndex = i;
        }
    }

    cache->blocks[index][lruIndex].valid = 1;
    cache->blocks[index][lruIndex].tag = tag;
    cache->blocks[index][lruIndex].lruCounter = 0;
    for (int i = 0; i < cache->associativity; i++) {
        if (i != lruIndex && cache->blocks[index][i].valid) {
            cache->blocks[index][i].lruCounter++;
        }
    }
    cache->misses++;
    return 0;
}

unsigned long hexToDecimal(char* hex) {
    return strtoul(hex, NULL, 16);
}

void runCacheSimulation(char* filename, int blockSize, int cacheSize, int associativity) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open trace file.\n");
        return;
    }

    Cache* cache = initCache(blockSize, cacheSize, associativity);
    char operation;
    char hexAddress[20];

    while (fscanf(file, " %c %s", &operation, hexAddress) == 2) {
        unsigned long address = hexToDecimal(hexAddress);
        accessCache(cache, address);

        if (operation == '1') cache->reads++;
        else if (operation == '2') cache->writes++;
    }

    fclose(file);
    printf("Reads: %d\n", cache->reads);
    printf("Writes: %d\n", cache->writes);
    printf("Hits: %d\n", cache->hits);
    printf("Misses: %d\n", cache->misses);
    float hitRate = (float)cache->hits / (cache->hits + cache->misses) * 100;
    printf("Hit Rate: %.2f%%\n", hitRate);

    freeCache(cache);
}

int main() {
    printf("Cache Simulator: Starting Simulation...\n");
    runCacheSimulation("trace.txt", 32, 4096, 2);
    runCacheSimulation("trace.txt", 64, 4096, 2);
    runCacheSimulation("trace.txt", 64, 4096, 1);
    runCacheSimulation("trace.txt", 64, 4096, 4);
    return 0;
}

// Block Size vs. Hit Rate:
// When block size increased from 32 to 64 bytes, hit rate improved from 88.2% to 91.7%.
// This shows that a larger block size increases spatial locality, allowing more data to be reused.
// However, too large of a block size could lead to fewer blocks total, limiting cache flexibility.

// Associativity vs. Hit Rate:
// With associativity 1 (direct mapped), hit rate was 78.5%.
// Increasing it to 2-way raised the hit rate to 91.7%, and with 4-way it reached 94.1%.
// This confirms that higher associativity reduces conflict misses and improves hit rate.