#include "main.h"

void ESC_Init()
{
	int mid_value=1500;
	
	int duty=0;
	
	uint32_t while_value = 50000;
	//激活电调
	duty = mid_value;

	do{
		while(while_value>0)
			while_value--;
		while_value = 50000;
		TIM_SetCompare1(TIM4,duty);
		TIM_SetCompare2(TIM4,duty);
		TIM_SetCompare3(TIM4,duty);
		TIM_SetCompare4(TIM4,duty);
		TIM_SetCompare3(TIM3,duty);
		TIM_SetCompare4(TIM3,duty);
		duty-=1;
	}while(duty>mid_value-100);
	do{
		while(while_value>0)
			while_value--;
		while_value = 50000;
		TIM_SetCompare1(TIM4,duty);
		TIM_SetCompare2(TIM4,duty);
		TIM_SetCompare3(TIM4,duty);
		TIM_SetCompare4(TIM4,duty);
		TIM_SetCompare3(TIM3,duty);
		TIM_SetCompare4(TIM3,duty);
		duty+=1;
	}while(duty<mid_value+300);
	
//	TIM_SetCompare1(TIM4,1500);
//	TIM_SetCompare2(TIM4,1500);//改为左上无刷电机
//	TIM_SetCompare3(TIM4,1500);
//	TIM_SetCompare4(TIM4,1500);
//	TIM_SetCompare3(TIM3,1500);
//	TIM_SetCompare4(TIM3,1500);
}



