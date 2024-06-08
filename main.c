#include "stm32f412zx.h"



//macros for trigger pin
#define Trig_high		GPIOA->BSRR=GPIO_BSRR_BS_0 			// turn on 	PA0 (trig pin)
#define Trig_low		GPIOA->BSRR=GPIO_BSRR_BR_0 			// turn off 	PA0 (trig pin)

uint32_t duration;
float distance;

//prototypes of the used function
void delaymS(uint32_t ms);
void delayuS(uint32_t us);
uint32_t read_echo(uint32_t timeout);


int main()
{

		 
	RCC->AHB1ENR = 0x3;		//enable GPIOB and GPIOA
	RCC->APB1ENR = 0x9;   //enable timer 2 and 5
	RCC->APB2ENR |= 0x1;  /Enable TIM3 clock/

	
	GPIOB->MODER |= 0x80; 		 									//PB3 FOR  PWM
	GPIOA->MODER |= 0X8; 			 									// PA1 FOR PWM	
	GPIOA->MODER	|=(1<<0);   									//set PA0 to Output
	//GPIOB->MODER |=0x1;
	 
	GPIOB->MODER |=0x5000;  									//PB6,PB7 SET AS OUTPUT MODE FOR 
	GPIOA->MODER |=0x10400;										//PA5,PA8 SET AS OUTPUT MODE 
	GPIOB->ODR =0x80;											 //FOR ORIENTATION OF FRONT WHEELS 
	GPIOA->ODR =0x20;												//for orientation of back wheel
			
	GPIOB->AFR[0]|= 0x1000; 								//Alternative function timer 2
	GPIOA->AFR[0] |=0X20;										//ALTERNATIVE FUNCTION FOR PA1 TIM5 AF2
		 
	//enable counter for delay in microsecond
	TIM1->PSC = 16 -1;  /* 16 000 000 /16 = 1000 000*/
	TIM1->ARR = 1;  /* 1000 000 /1 = 1000000*/
	TIM1->CNT =0;
	TIM1->CR1 =1;

	//TIM2
	TIM2->PSC = 39;
	TIM2->ARR = 3999;
	TIM2->CCMR1 |= 0x6000;
	TIM2->CCER |= 0x10;
	TIM2->CCR2 =2000;
	TIM2->CR1 |=0x1;
	TIM2->SR = 0;
	//TIM5
	TIM5->PSC=39;
	TIM5->ARR=3999;
	TIM5->CCMR1 |=0X6000;
	TIM5->CCER |= 0x10;
	TIM5->CCR2 =2000;
	TIM5->CR1 |=0x1;
	TIM5->SR = 0;
	
	while(1)
	{
	Trig_low; 														//turn off trig
	delayuS(10); 													//wait 4uS
	Trig_high;  													//turn on trig
	delayuS(10); 
	Trig_low;
	duration=read_echo(400000); 								    //measure the time of echo pin
	distance=duration/58;										  			//distance=duration/2*SOUND_SPEED
	//delaymS(1000);
		
	GPIOB->ODR |=0X80;														 //FOR ORIENTATION OF FRONT WHEELS 
	GPIOA->ODR |=0x100;															//for orientation of back wheel	
	 if(distance<8){
		 
		GPIOB->ODR |=0X80;													 //FOR ORIENTATION OF FRONT WHEELS 
		GPIOA->ODR |=0x20;														//for orientation of back wheel 
		TIM2->CCR2 = 2000;
		TIM5->CCR2 = 2000;
		
	 
	 }
	 else{
		
		 
		TIM2->CCR2 = 2000;
		TIM5->CCR2 = 2000;
		GPIOB->ODR |=0X80;											 //FOR ORIENTATION OF FRONT WHEELS 
		GPIOA->ODR |=0x20;												//for orientation of back wheel
		
	 }
		 
	}

	}
	
	
	
void delaymS(uint32_t ms) //delay for certain amount in milliseconds
	{
	SysTick->LOAD=16000-1;
	SysTick->VAL=0;
	SysTick->CTRL=0x5;
		for (int i=0;i<ms;i++)
		{
			while(!(SysTick->CTRL &0x10000)){}
		}
	SysTick->CTRL=0;
	
	}
	
void delayuS(uint32_t us) //delay for certain amount in microseconds
	{
for(int i =0;i<us;i++){
	
		 while(!(TIM1->SR & 1)){}   /wait for UIF set/
			 TIM1->SR &= ~1;
			 
	}
	}
	
uint32_t read_echo(uint32_t timeout)
	{
		uint32_t duration;
	while(!((GPIOA->IDR)&GPIO_IDR_ID2)){duration++;delayuS(1);
	if(duration>timeout){return 0;}
	}
	duration=0;
while((GPIOA->IDR&GPIO_IDR_ID2)){duration++;delayuS(1);if(duration>timeout){return 0;} }
	return duration;
	}
