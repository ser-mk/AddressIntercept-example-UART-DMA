#include <stm32f10x.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


addr_t pAddrSRAM = NULL;
addr_t pAddrPERIPH = NULL;


static const size_t QTY_REMOTE_REGION = 1U;
static const addr_t REMOTE_PERIPHERAL_ADDR = (addr_t)0x40000000U;
static const size_t REMOTE_PERIPHERAL_SIZE = 0x30000U;

static const addr_t REMOTE_MEMORY_ADDR = (addr_t)0x20000000U;
static const size_t REMOTE_MEMORY_SIZE = 96*1024;


static memoryTranslate * memoryMap = NULL;
static sizeMemoryTranslate_t sizeMemoryMap = 0;

static void setAddr(memoryTranslate *map, size_t size, addr_t remote_addr){
    map->start_addr = malloc(size);
    map->end_addr = memoryMap->start_addr + size;
    map->reference_addr = remote_addr;
}

memoryTranslate * getMemoryMap(sizeMemoryTranslate_t * size){
    memoryMap = (memoryTranslate *)malloc(sizeof(memoryTranslate)*QTY_REMOTE_REGION);

    setAddr(memoryMap, REMOTE_PERIPHERAL_SIZE, REMOTE_PERIPHERAL_ADDR);

    setAddr(memoryMap + 1, REMOTE_MEMORY_SIZE, REMOTE_MEMORY_ADDR);

    *size = sizeMemoryMap = QTY_REMOTE_REGION;

    printf("memoryMap = %p %p s = %p\n", memoryMap, memoryMap + 1, size);

    return memoryMap;
}


int main()
{

    sizeMemoryTranslate_t s = 0;

        memoryTranslate * p = getMemoryMap(&s);

        printf("size : %d  p : %p\n", s, p);

        pAddrPERIPH = p[0].start_addr;

            pAddrSRAM = p[1].start_addr;

            printf("!RCC %p\n", RCC);


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	
    gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
	for(;;)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        sleep(1);

        printf("toogle\n");
		
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        sleep(1);
	}
	return 0;
}

