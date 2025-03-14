#include "stm32f10x.h"

int counter = 0;

void SysTick_Handler ( void ) {
		counter ++;
}

int main ( void )
{
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 3) | (0x01 << 4) ;
	
	GPIOA->CRL &= ~(0xF<<4) ;
	GPIOA->CRL |= (0x2<<4) ;
	
	// ensruing that systick is disabled
	SysTick ->CTRL = 0x0;
	
	// Systick will reload after 1ms
	SysTick -> LOAD = (SystemCoreClock / 1000U);
	
	// loaded value after reaching zero
	SysTick ->VAL = 100  ;
	
	// enable interruption
	SysTick -> CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// enabling systick
	SysTick -> CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk  ;
	
	while (1)
	{
		
	}
}
