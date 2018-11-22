#include <stm32f10x.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


addr_t pAddrSRAM = NULL;
addr_t pAddrPERIPH = NULL;


static const size_t QTY_REMOTE_REGION = 2U;
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


#define USARTy                   USART1
#define USARTy_GPIO              GPIOA
#define USARTy_CLK               RCC_APB2Periph_USART1
#define USARTy_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTy_RxPin             GPIO_Pin_10
#define USARTy_TxPin             GPIO_Pin_9
#define USARTy_Tx_DMA_Channel    DMA1_Channel4
#define USARTy_Tx_DMA_FLAG       DMA1_FLAG_TC4
#define USARTy_Rx_DMA_Channel    DMA1_Channel5
#define USARTy_Rx_DMA_FLAG       DMA1_FLAG_TC5
#define USARTy_DR_Base           0x40013804

#define USARTz                   USART2
#define USARTz_GPIO              GPIOA
#define USARTz_CLK               RCC_APB1Periph_USART2
#define USARTz_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTz_RxPin             GPIO_Pin_3
#define USARTz_TxPin             GPIO_Pin_2
#define USARTz_Tx_DMA_Channel    DMA1_Channel7
#define USARTz_Tx_DMA_FLAG       DMA1_FLAG_TC7
#define USARTz_Rx_DMA_Channel    DMA1_Channel6
#define USARTz_Rx_DMA_FLAG       DMA1_FLAG_TC6
#define USARTz_DR_Base           0x40004404

const uint8_t * addrDMAbuf = 0x20000000;
const size_t sizeDMAbuf = 0x200;

void init(void);

int main()
{

    sizeMemoryTranslate_t s = 0;

        memoryTranslate * p = getMemoryMap(&s);

        printf("size : %d  p : %p\n", s, p);

        pAddrPERIPH = p[0].start_addr;

            pAddrSRAM = p[1].start_addr;

            //printf("!RCC %p\n", RCC);
            *(int*)pAddrSRAM = 0;

    init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	
    gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
    for(int i = 0; i < sizeDMAbuf; i++)
    {
        const uint8_t sendByte = i & 0x7F;
    /* Send one byte from USARTy to USARTz */
        USART_SendData(USARTy, sendByte);

        /* Loop until USARTy DR register is empty */
        while(USART_GetFlagStatus(USARTy, USART_FLAG_TXE) == RESET){}

        printf("send byte %x ...\n", sendByte);

        /* Loop until the USARTz Receive Data Register is not empty */
        //while(USART_GetFlagStatus(USARTz, USART_FLAG_RXNE) == RESET){}

        const uint16_t rec =  DMA_GetCurrDataCounter(USARTz_Rx_DMA_Channel);

        printf("... received byte %d : ", rec);
        const uint8_t * pM = (uint8_t *)pAddrSRAM;
        for(int r = 0; r < 0x20; r++){
            printf(" %x,", pM[r]);
        }
        printf("\n");

        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        sleep(1);

        printf("toogle\n");
		
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        sleep(1);
	}
	return 0;
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{

    /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | USARTz_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable USARTy Clock */
  RCC_APB2PeriphClockCmd(USARTy_CLK, ENABLE);

  /* Enable USARTz Clock */
  RCC_APB1PeriphClockCmd(USARTz_CLK, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USARTy Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USARTy_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);

  /* Configure USARTz Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USARTz_RxPin;
  GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);

  /* Configure USARTy Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USARTy_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);

  /* Configure USARTz Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USARTz_TxPin;
  GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
  //DMA_DeInit(USARTy_Tx_DMA_Channel);

  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#if 0
  DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);

  /* USARTy RX DMA1 Channel (triggered by USARTy Rx event) Config */
  DMA_DeInit(USARTy_Rx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize2;
  DMA_Init(USARTy_Rx_DMA_Channel, &DMA_InitStructure);

  /* USARTz TX DMA1 Channel (triggered by USARTz Tx event) Config */
  DMA_DeInit(USARTz_Tx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTz_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer2;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = TxBufferSize2;
  DMA_Init(USARTz_Tx_DMA_Channel, &DMA_InitStructure);
#endif
  /* USARTz RX DMA1 Channel (triggered by USARTz Rx event) Config */
  DMA_DeInit(USARTz_Rx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTz_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)addrDMAbuf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = sizeDMAbuf;
  DMA_Init(USARTz_Rx_DMA_Channel, &DMA_InitStructure);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */

void init(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
           this is done through SystemInit() function which is called from startup
           file (startup_stm32f10x_xx.s) before to branch to application main.
           To reconfigure the default setting of SystemInit() function, refer to
           system_stm32f10x.c file
         */
    USART_InitTypeDef USART_InitStructure;
      /* System Clocks Configuration */
      RCC_Configuration();

      /* Configure the GPIO ports */
      GPIO_Configuration();

      /* Configure the DMA */
      DMA_Configuration();

    /* USARTy and USARTz configuration ------------------------------------------------------*/
      /* USARTy and USARTz configured as follow:
            - BaudRate = 230400 baud
            - Word Length = 8 Bits
            - One Stop Bit
            - Even parity
            - Hardware flow control disabled (RTS and CTS signals)
            - Receive and transmit enabled
      */
      USART_InitStructure.USART_BaudRate = 230400;
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      USART_InitStructure.USART_Parity = USART_Parity_Even;
      USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

      /* Configure USARTy */
      USART_Init(USARTy, &USART_InitStructure);
      /* Configure USARTz */
      USART_Init(USARTz, &USART_InitStructure);

      /* Enable USARTz DMA Rx and TX request */
      USART_DMACmd(USARTz, USART_DMAReq_Rx, ENABLE);

      /* Enable USARTz RX DMA1 Channel */
      DMA_Cmd(USARTz_Rx_DMA_Channel, ENABLE);

      /* Enable the USARTy */
      USART_Cmd(USARTy, ENABLE);

      /* Enable the USARTz */
      USART_Cmd(USARTz, ENABLE);

}
