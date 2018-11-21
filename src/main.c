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


#define USARTy                   USART1
#define USARTy_GPIO              GPIOA
#define USARTy_CLK               RCC_APB2Periph_USART1
#define USARTy_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTy_RxPin             GPIO_Pin_10
#define USARTy_TxPin             GPIO_Pin_9

#define USARTz                   USART2
#define USARTz_GPIO              GPIOA
#define USARTz_CLK               RCC_APB1Periph_USART2
#define USARTz_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTz_RxPin             GPIO_Pin_3
#define USARTz_TxPin             GPIO_Pin_2

void init(void);

int main()
{

    sizeMemoryTranslate_t s = 0;

        memoryTranslate * p = getMemoryMap(&s);

        printf("size : %d  p : %p\n", s, p);

        pAddrPERIPH = p[0].start_addr;

            pAddrSRAM = p[1].start_addr;

            printf("!RCC %p\n", RCC);

    init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	
    gpio.GPIO_Pin = GPIO_Pin_13;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
	for(;;)
    {
        const uint8_t sendByte = 0x75;
    /* Send one byte from USARTy to USARTz */
        USART_SendData(USARTy, sendByte);

        /* Loop until USARTy DR register is empty */
        while(USART_GetFlagStatus(USARTy, USART_FLAG_TXE) == RESET)
        {
        }

        printf("send byte %x ...\n", sendByte);

        /* Loop until the USARTz Receive Data Register is not empty */
        while(USART_GetFlagStatus(USARTz, USART_FLAG_RXNE) == RESET)
        {
        }

        /* Store the received byte in RxBuffer */
        const uint8_t rByte = (USART_ReceiveData(USARTz) & 0x7F);

        printf("... receive byte %x\n", rByte);


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

      /* Enable the USARTy */
      USART_Cmd(USARTy, ENABLE);

      /* Enable the USARTz */
      USART_Cmd(USARTz, ENABLE);
}
