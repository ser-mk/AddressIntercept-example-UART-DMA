#include <stm32f10x.h>

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	
	gpio.GPIO_Pin = GPIO_Pin_8; 
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
	for(;;)
	{
		volatile unsigned long i;
		
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		for (i = 1000000; i > 0; i--);
		
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		for (i = 1000000; i > 0; i--);
	}
	return 0;
}

