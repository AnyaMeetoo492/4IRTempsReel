#include "stm32f10x.h"

int counter = 0;

// Each register 32 bits
// [xPSR, PC, LR, R12, R3, R2, R1, R0]

int PSP = 0;

// NOTE : Stack is 0x5000 in size and starts at 0x200000000
// stack pointers start at address ( remember stack decrements ) 
uint32_t * stack1 = (uint32_t *) 0x20004000 ;
uint32_t * stack2 = (uint32_t *) 0x20005000 ;

int current_process;

void SysTick_Handler ( void ) {
		if (current_process == 0) { // process 1
			__set_PSP( (uint32_t) stack1);
			current_process = 1;
		}
		else{ // process 2 
			__set_PSP( (uint32_t) stack2);
			current_process = 0;
		}
		
		counter ++;
}

void dummy1 ( void ) {
	while (1){
	}
}

void dummy2 ( void ) {
	while (1){
	}
}

int main ( void )
{
	RCC->APB2ENR |= (0x01 << 2) | (0x01 << 3) | (0x01 << 4) ;
	
	GPIOA->CRL &= ~(0xF<<4) ;
	GPIOA->CRL |= (0x2<<4) ;
	
	// check if not already in unpriviledge mode, Process stack
	if (!(__get_CONTROL()&(0b11))){
		__set_CONTROL(0b11);
	}
	
	// start at process 1
	current_process = 0;
	
	// xPSR 0x1000000
	*stack1 = 0x1000000;
	*stack2 = 0x1000000;
	
	// PC to address of respective function
	*(stack1 + 1) =  (uint32_t) &dummy1;
	*(stack2 + 1) =  (uint32_t) &dummy2;
	
	// LR back to thread mode with process stack 0xFFFFFFFD 
	*(stack1 + 2) =  (uint32_t) 0xFFFFFFFD;
	*(stack2 + 2) =  (uint32_t) 0xFFFFFFFD;
	
	// initialise R0-R3 and R12 to 0x0
	for (int i=3; i<8; i++){
		*(stack1 + i) = 0x0;
		*(stack2 + i) = 0x0;
	}
	
	// ensruing that systick is disabled
	SysTick ->CTRL = 0x0;
	
	// Systick will reload after 1ms
	SysTick -> LOAD = (SystemCoreClock / 1000U);
	
	// loaded value after reaching zero
	SysTick ->VAL = 10;
	
	// enable interruption
	SysTick -> CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// enabling systick
	SysTick -> CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk  ;
	
	
	while (1)
	{
		
	}
}
