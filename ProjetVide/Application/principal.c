#include "stm32f10x.h"
#include "stdbool.h"

int counter = 0;
int counter1 = 0;
int counter2 = 0;

int time = 0;

// Each register 32 bits
// [xPSR, PC, LR, R12, R3, R2, R1, R0]

int PSP = 0;

// NOTE : Stack is 0x5000 in size and starts at 0x200000000
// stack pointers start at address ( remember stack decrements ) 
uint32_t * stack1 = (uint32_t *) 0x20004000 ;
uint32_t * stack2 = (uint32_t *) 0x20003000 ;

uint32_t * previous_process_address;
uint32_t * next_process_address;

bool setup = false;

int current_process;

// init table scheduler
uint32_t schedule[40] = {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0};

void PendSV_Handler ( void ) {
	
	// Stack r4 of previous process
	if (current_process == 1){
		 previous_process_address = stack2;
	}
	else if (current_process == 2) {
		previous_process_address = stack1;
	}
	
	__ASM volatile ("MOV r0, %0" :: "r" (previous_process_address):); // copy previous stack address 
	__ASM volatile ("SUBS r0, #4"); // go to R4 address
	__ASM volatile ("STR r4, [r0]"); // stack R4 

	// Pop r4 for next process
	if (current_process == 1){
		 next_process_address = stack1;
	}
	else if (current_process == 2) {
		next_process_address = stack2;
	}
	__ASM volatile ("MOV r0, %0" :: "r" (next_process_address):); // copy stack of next process
	__ASM volatile ("SUBS r0, #4"); // go to R4 address in stack
	__ASM volatile ("LDR r4, [r0]"); // stack R4
	
}

void SysTick_Handler ( void ) {
	
		if (!setup) {
			__set_PSP((uint32_t) stack1);
			setup = true;
			current_process = 1;
			return;
		}
		
		time = (time +1)%40;
		current_process = schedule[(time+40-1)%40] + 1; 
		
		if (schedule[time] != schedule[(time+40-1)%40]){
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
			
			// trigger PendSV
			SCB -> ICSR |= SCB_ICSR_PENDSVSET ;
		}
		

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
	
	// setting priorities for Systick and PendSV
	NVIC_SetPriority ( SysTick_IRQn , 0x00 ) ;
	NVIC_SetPriority ( PendSV_IRQn , 0x03 ) ;

	
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
	SysTick -> LOAD = (SystemCoreClock /5000);
	
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
