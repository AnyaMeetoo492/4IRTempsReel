#include "stm32f10x.h"
#include "stdbool.h"

int counter = 0;
int counter1 = 0;
int counter2 = 0;

// Each register 32 bits
// [xPSR, PC, LR, R12, R3, R2, R1, R0]

int PSP = 0;

// NOTE : Stack is 0x5000 in size and starts at 0x200000000
// stack pointers start at address ( remember stack decrements ) 
uint32_t * stack1 = (uint32_t *) 0x20004000 ;
uint32_t * stack2 = (uint32_t *) 0x20003000 ;

bool setup = false;

int current_process;

void SysTick_Handler ( void ) {
		if (!setup) {
			__set_PSP((uint32_t) stack1);
			setup = true;
			current_process = 1;
			return;
		}
	
		if (current_process == 1) { // process 1
			stack1 = (uint32_t *) __get_PSP();
			__set_PSP((uint32_t) stack2);
			current_process = 2;
		}
		else{ // process 2 
			stack2 = (uint32_t *) __get_PSP();
			__set_PSP( (uint32_t)stack1);
			current_process = 1;
		}
		counter ++;
}

void dummy1 ( void ) {
	while (1){
		counter1++;
	}
}

void dummy2 ( void ) {
	while (1){
		counter2++;
	}
}

int main ( void )
{
	// start at process 1
	current_process = 0;
	
	// xPSR 0x1000000
	*(stack1 + 7) = 0x1000000;
	*(stack2 + 7) = 0x1000000;
	
	// PC to address of respective function
	*(stack1 + 6) =  (uint32_t) &dummy1;
	*(stack2 + 6) =  (uint32_t) &dummy2;
	
	// LR back to thread mode with process stack 0xFFFFFFFD 
	*(stack1 + 5) =  (uint32_t) 0xFFFFFFFD;
	*(stack2 + 5) =  (uint32_t) 0xFFFFFFFD;
	
	// initialise R0-R3 and R12 to 0x0
	for (int i=0; i<5; i++){
		*(stack1 + i) = 0x0;
		*(stack2 + i) = 0x0;
	}
	
	// Systick will reload after 1ms
	SysTick -> LOAD = (SystemCoreClock / 1000U);
	
	// enable interruption
	SysTick -> CTRL |= SysTick_CTRL_TICKINT;
	
	// enabling systick
	SysTick -> CTRL |= SysTick_CTRL_CLKSOURCE;
	SysTick -> CTRL |= SysTick_CTRL_ENABLE  ;
	
	// check if not already in unpriviledge mode, Process stack
	uint32_t ctrl = __get_CONTROL();
	ctrl |= 0b11;
	__set_CONTROL(ctrl);
	
	
	while (1)
	{
		
	}
}
