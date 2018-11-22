#include "addrIntercept.h"
#include <stdlib.h>

addr_t pAddrSRAM = NULL;
addr_t pAddrPERIPH = NULL;


#define  QTY_REMOTE_REGION  ((size_t)2U)
static const addr_t REMOTE_PERIPHERAL_ADDR = (addr_t)0x40000000U;
#define REMOTE_PERIPHERAL_SIZE  ((size_t)0x30000U)

static const addr_t REMOTE_MEMORY_ADDR = (addr_t)0x20000000U;
#define REMOTE_MEMORY_SIZE ((size_t)96*1024)

static char PERIPHERAL_BUFFER[REMOTE_PERIPHERAL_SIZE];
static char MEMORY_BUFFER[REMOTE_MEMORY_SIZE];

static memoryTranslate memoryMap[QTY_REMOTE_REGION];
static sizeMemoryTranslate_t sizeMemoryMap = 0;

static void setAddr(memoryTranslate * const map, const char * buf, const size_t size, const addr_t remote_addr){
    map->start_addr = (addr_t)buf;
    map->end_addr = memoryMap->start_addr + size;
    map->reference_addr = remote_addr;
}

memoryTranslate * getMemoryMap(sizeMemoryTranslate_t * size){

    setAddr(memoryMap, PERIPHERAL_BUFFER, REMOTE_PERIPHERAL_SIZE, REMOTE_PERIPHERAL_ADDR);

    setAddr(memoryMap + 1, MEMORY_BUFFER, REMOTE_MEMORY_SIZE, REMOTE_MEMORY_ADDR);

    *size = sizeMemoryMap = QTY_REMOTE_REGION;

    return memoryMap;
}
